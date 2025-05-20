<?php
/**\addtogroup comment
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

class CommentAdapter
{
	var $component;
	var $parentClass;
	var $xrefClass;
	var $imported = false;
	
	function __construct($component, $parentClass, $xrefClass)
	{
		$this->component   = $component;
		$this->parentClass = $parentClass;
		$this->xrefClass   = $xrefClass;
	}

	function importComponent()
	{
		if (!$this->imported)
		{
			Fakoli::using($this->component);
			$this->imported = true;
		}
	}
	
	function createParent()
	{
		$this->importComponent();
		$parent = new $this->parentClass;
		if (isset($_GET[$parent->getPrimaryKey()]) && checkNumeric($_GET[$parent->getPrimaryKey()]))
		{
			$parent->load($_GET[$parent->getPrimaryKey()]);
		}
		
		return $parent; 
	}
	
	function createXref()
	{
		$this->importComponent();		
		return new $this->xrefClass;
	}	
}

class CommentManager
{	
	static $adapters = array();
	
	function __construct()
	{
		
	}
	
	static function setDefaults()
	{		
		Settings::setDefaultValue("comment", "allow_anonymous_commenting", false, Boolean, "", "Commenting Rules");
		Settings::setDefaultValue("comment", "publish_automatically", true, Boolean, "", "Commenting Rules");
		Settings::setDefaultValue("comment", "use_captcha", "Never", String, "Specify when to use a Captcha to verify the user", "Commenting Rules", "Never\nAnonymous Only\nAlways");
		
		Settings::setDefaultValue("comment", "enable_item_rating", false, Boolean, "Specifies whether to display the rating control", "Comment Configuration", null, 2);
		Settings::setDefaultValue("comment", "show_subject_field", true, Boolean, "Specifies whether users can have a title as part of their comment", "Comment Configuration", null, 1);
		
		Settings::setDefaultValue("comment", "username_format", "Full Name", String, 
			"Specifies whether to display the full name or username (handle) of logged in commenters", "Comment Configuration", "Full Name\nUsername", 3);
		
		Settings::setDefaultValue("comment", "anonymous_preamble_text", "", String, 
					"Identifier of a Text Lookup item containing preamble text that appears at the top of the comment panel for anonymous users", 
					"Comment Configuration", null, 4);
		
		Settings::setDefaultValue("comment", "logged_in_preamble_text", "", String,
					"Identifier of a Text Lookup item containing preamble text that appears at the top of the comment panel for logged in users",
					"Comment Configuration", null, 5);
		
		CommentManager::setDefaultEmailTemplates();
		CommentManager::setDefaultMergeCodes();
	}
	

	/**
	 * For comment, need default template:
	 * new_comment
	 */
	static function setDefaultEmailTemplates()
	{
		global $config;
	
		$sitename = $config["sitename"];
		$contact = $config["email_contact"];
		
		$message = "This is an automated message from the {$sitename}.<br><br>A new comment has been posted. To review and publish this comment, click the link below:<br><br>[article_comment_url]";
		EmailTemplate::setDefaultTemplate("new_comment", $contact, "[article_title] Comment ([comment_title])", $message, "ArticleCommentXref");
	}
		
	/**
	 * Merge codes for ArticleCommentXref:
	 * comment_article_id
	 * comment_title 
	 * article_title
	 * article_url
	 */
	static function setDefaultMergeCodes()
	{
		MergeCode::setDefaultMergeCode("comment_article_id", "Article.article_id", "ArticleCommentXref", "Comment Article ID");
		MergeCode::setDefaultMergeCode("comment_title", "Comment.title", "ArticleCommentXref", "Comment Title");
		MergeCode::setDefaultMergeCode("article_title", "Article.title", "ArticleCommentXref", "Article Title");
		MergeCode::setDefaultMergeCode("article_url", "Article.getUrl", "ArticleCommentXref", "URL of article with list of comments");
		MergeCode::setDefaultMergeCode("article_comment_url", "getUrl", "ArticleCommentXref", "URL of article with list of comments");
	}
	
	static function onInitialize()
	{
	}
	
	static function upgradeComponent($version)
	{
		$mgr = new CommentUpgradeManager();
		$mgr->upgrade($version);
	}
	
	static function registerAdapter($component, $parentClass, $xrefClass)
	{
		trace("Registering CommentAdapter for $component", 3);
		$adapter = new CommentAdapter($component, $parentClass, $xrefClass);		
		$proto = $adapter->createParent();
		CommentManager::$adapters[$proto->getPrimaryKey()] = $adapter;
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
		
		trace("Component comment is deleting objects dependent on user_id {$user_id}", 3);
	
		$comment = new Comment();
		$comment->delete("WHERE user_id={$user_id}");
		
		return $user;
	}
	
	static function getAdapters()
	{
		if (count(CommentManager::$adapters) == 0)
		{
			ComponentManager::fireEvent("RegisterCommentAdapters");
		}
		return CommentManager::$adapters;
	}
	
	static function getAdapter()
	{
		$adapters = CommentManager::getAdapters();
		
		trace(print_r($adapters, true), 3);
		
		foreach($_GET as $key => $value)
		{
			if (array_key_exists($key, $adapters))
			{
				return $adapters[$key];
			}
		}
	}
}
/**@}*/?>
