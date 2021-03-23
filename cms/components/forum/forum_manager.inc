<?php
/**\addtogroup forum
 * @{
 */


/**************************************************************

 Copyright (c) 2010 Sonjara, Inc

 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:

 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.

 Except as contained in this notice, the name(s) of the above 
 copyright holders shall not be used in advertising or otherwise 
 to promote the sale, use or other dealings in this Software 
 without prior written authorization.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.

*****************************************************************/


class ForumManager
{
	function ForumManager()
	{
		
	}
	
	static $userProfileHandler = null;
	
	static function initializeUserProfileHandler()
	{
		if (!ForumManager::$userProfileHandler)
		{
			ForumManager::$userProfileHandler = array(ForumManager, defaultUserProfileHandler);
			ComponentManager::fireEvent("RegisterForumUserProfileHandler");
		}
	}
	
	static function registerUserProfileHandler($handler)
	{
		ForumManager::$userProfileHandler = $handler;
	}
	
	static function formatUserProfile($u)
	{
		ForumManager::initializeUserProfileHandler();
		return call_user_func_array(ForumManager::$userProfileHandler, array($u));
	}
		
	static function defaultUserProfileHandler($user)
	{
		return $user->getFullName();
	}
	
	static function displayForum($identifier, &$continue)
	{
		global $page_role;
		
		try
		{
			$forum = Forum::findByIdentifier($identifier);
			$page = ComponentPage::findByIdentifier("forum_details", "WHERE enabled=1");
			$_GET["forum_id"] = $forum->forum_id;
		
			$pageView = new ComponentPageView($page, "{$page->template}.tpl");
		
			$page_role = $page->role;
			
			if (!checkRole($page->role))
			{
				LoginManager::storeRedirectPage();
				redirect("/login");
			}
			
			echo $pageView->drawView();
			
			$continue = false;
		}
		catch(DataNotFoundException $e)
		{
			
		}
		
		return $identifier;
	}
	
	static function enumerateFora($items)
	{
		$pages = Query::create(Forum, "WHERE identifier IS NOT NULL AND identifier != '' ORDER BY title")->execute();
		
		$items["Discussion Forums"] = $pages;
		return $items;
	}

	static function setDefaults()
	{
		Settings::setDefaultValue("forum", "forum_list_items_per_page", 0, Number, "Number of items per page on the forum list (0 for no pagination)");
		Settings::setDefaultValue("forum", "forum_topics_per_page", 0, Number, "Number of topics per page on the forum list (0 for no pagination)");
	}
	
	
	/**
	 * Respond to fired event DeleteUser
	 *
	 * Delete any records in this component that have dependencies on user object.
	 *
	 * @param obj $user - class SiteUser or custom user class
	 */
	static function deleteUser($user)
	{
		$pk = $user->getPrimaryKey();
		$user_id = $user->$pk;
		
		trace("Component forum is deleting objects dependent on user_id {$user_id}", 3);
	
		$tx = new DataTransaction();
		
		try
		{
			$forums = Query::create(Forum, "WHERE owner_id=:owner_id")
					->bind(":owner_id", $user_id)
					->execute();
			
			if(count($forums))
			{
				foreach($forums as $forum)
				{
					$forum->joinTransaction($tx);
					ForumManager::deleteForum($forum);
				}
			}
			
			$messages = Query::create(ForumMessage, "WHERE author_id=:author_id")
				->bind(":author_id", $user_id)
				->execute();
			
			if(count($messages))
			{
				foreach($messages as $message)
				{				
					$message->joinTransaction($tx);
					ForumManager::deleteForumMessage($message);
				}
			}	
			
			$tx->commit();
		}
		catch(Exception $e)
		{
			$tx->rollback();
			throw $e;
		}
		
		return $user;
	}
	
	/**
	 * When deleting a forum, we must delete topics, messages, attachments and
	 * site xrefs.
	 * 
	 * @param unknown $forum
	 * @throws Exception
	 */
	static function deleteForum($forum)
	{
		$forum_id = $forum->forum_id;
		
		$tx = new DataTransaction();
		
		try
		{
			$forum->joinTransaction($tx);
			
			$topic = new ForumTopic();
			$topic->joinTransaction($tx);
			$topic->delete("WHERE forum_id={$forum_id}");
			
			$messages = $forum->Messages();
			
			if(count($messages))
			{
				foreach($messages as $message)
				{
					$message->joinTransaction($tx);
					ForumManager::deleteForumMessage($message);
				}
			}
			
			$xref = new ForumSiteXref();
			$xref->joinTransaction($tx);
			$xref->delete("WHERE forum_id={$forum_id}");
			
			$forum->delete();
			
			$tx->commit();
		}
		catch(Exception $e)
		{
			$tx->rollback();
			throw $e;
		}
	}
	
	static function deleteForumMessage($message)
	{
		$tx = new DataTransaction();
		
		try
		{
			$message->joinTransaction($tx);
			
			$attachments = $message->Attachments();
			if(count($attachments))
			{
				$xref = new ForumMessageAttachmentXref();
				$xref->joinTransaction($tx);
				
				foreach($attachments as $attachment)
				{
					$attachment->joinTransaction($tx);
					$attachment->delete();
					$xref->delete("WHERE attachment_id={$attachment->attachment_id}");
				}		
			}
			$message->delete();		
				
			$tx->commit();
		}
		catch(Exception $e)
		{
			$tx->rollback();
			throw $e;
		}				
	}
	
	static function upgradeComponent($version)
	{
		$mgr = new ForumUpgradeManager();
		$mgr->upgrade($version);
	}

	static function registerTaxonomyClasses($classes)
	{
		$classes[] = Forum;
		return $classes;
	}
}
/**@}*/?>