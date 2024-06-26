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

Fakoli::usingFile("framework/data_view.inc");

class ForumListView
{
	var $fora;
	var $id;
	var $table;
	var $forumLink;
	var $messageLink;
	var $reorder;
	
	function ForumListView($fora, $forumLink, $messageLink, $id = "forum_list", $reorder = false)
	{
		$this->fora = $fora;
		$this->id = $id;
		
		$this->forumLink = $forumLink;
		$this->messageLink = $messageLink;
		
		$this->table = new DataListView($fora, $id);
		
		
		$this->table->column("Title", array($this, formatTitle), true, "width: 40%")
			 		->column("Last Post", array($this, formatLastPost), true, "width: 40%")
			 		->column("Topics", "{topics}", true, "width: 10%;text-align: center")
			 		->column("Posts", "{posts}", true, "width: 10%;text-align: center");
			 		
		if (checkRole("admin") && $reorder)
		{
			$this->table->enableDragReorder("/action/forum/reorder_forums");
		}
		else
		{
			$this->table->pageSize = Settings::getValue("forum", "forum_list_items_per_page");
			$this->table->filter = true;
		}
		
		$this->table->emptyMessage = "No fora have been created";
	}
	
	function formatTitle($item)
	{
		$text = $item->format("<a href='{$this->forumLink}?forum_id={forum_id}'>{title|No Title}</a>");
		if ($item->teaser)
		{
			$text .= "<br/><span class='small'>{$item->teaser}</span>";
		}
		return $text;
	}
	
	function formatLastPost($item)
	{
		$lastPost = $item->LastPost();
		if (!$lastPost) return "<em>No posts</em>";
		$user = $lastPost->Author();
		$userProfile = ForumManager::formatUserProfile($user);
		
		$title = $lastPost->title ? $lastPost->title : "No Title";
		
		return "<a href='{$this->messageLink}?forum_id={$lastPost->forum_id}&topic_id={$lastPost->topic_id}'>{$title}</a><br/>".
			   "<span class='small'>by ".$userProfile." ({$lastPost->date_posted})</span>";
		
	}
	
	function writeScript()
	{
		return $this->table->writeScript();
	}
	
	function drawView()
	{
		$this->table->drawView();
	}
}


class ForumDetailsView
{
	var $forum;
	var $topics;
	var $messages;
	var $table;
	var $topicLink;
	var $width;
	var $subscriptionView;
	
	function ForumDetailsView($forum, $topicLink)
	{
		$this->forum = $forum;
		$this->topicLink = $topicLink;
			
		$msgTable = new ForumMessage();
		$this->topics = $this->forum->Topics("WHERE message_id not in (select message_id from {$msgTable->table} where deleted=true)");
		$this->messages = indexedQuery(ForumMessage, "WHERE forum_id={$forum->forum_id} AND deleted=false AND parent_id=0", "topic_id");
		
		$this->table = new DataListView($this->topics, "topic_list");
		$this->table->column("Title", array($this, formatTitle), true, "width: 40%")
					->column("Last Post", array($this, formatLastPost), true, "width: 40%")
					->column("Views", "{views}", true, "width: 10%; text-align: center")
					->column("Replies", "{countReplies()}", true, "width: 10%; text-align: center");
		$this->table->emptyMessage = "No topics have been started in this discussion forum";
		$this->table->filter = true;
		$this->table->pageSize = Settings::getValue("forum", "forum_topics_per_page");
		
		$this->subscriptionView = new ForumSubscriptionHelper($this->forum);
		
	}
	
	function formatTitle($topic)
	{
		if (!array_key_exists($topic->topic_id, $this->messages)) return "";
	
		$message = $this->messages[$topic->topic_id];
		$user = $message->Author();
		$userProfile = ForumManager::formatUserProfile($user);

		$title = $message->title ? $message->title : "No Title";
		
		return "<a href='{$this->topicLink}?forum_id={$topic->forum_id}&topic_id={$topic->topic_id}'>{$title}</a><br/><span class='small'>(Created by ".$userProfile.")</span>";
	}
	
	function formatLastPost($topic)
	{
		$lastPost = $topic->LastPost();
		if (!$lastPost) return "<em>No posts</em>";
		$user = $lastPost->Author();
		
		$userProfile = ForumManager::formatUserProfile($user);

		$title = $lastPost->title ? $lastPost->title : "No Title";
		
		return "<a href='{$this->topicLink}?forum_id={$lastPost->forum_id}&topic_id={$lastPost->topic_id}'>{$title}</a><br/>".
			   "<span class='small'>by ".$userProfile." ({$lastPost->date_posted})</span>";
	}
	
	function writeScript()
	{
		$script = $this->table->writeScript();
		$script .= $this->subscriptionView->writeScript();
		
		return $script;
	}
	
	function drawView()
	{		
		echo "<div style='width: {$this->width}'>";
?>
	<form method="get" action="forum_search_by_author">
	<input type="hidden" name="forum_id" value="<?echo $this->forum->forum_id?>"/>
	<label for="author">Search by Author</label> <input type="text" name="author" style="width: 200px"> <input class='button' type='submit' value="Search"/>
	</form>
<?		
		$this->subscriptionView->drawLink();
		$this->table->drawView();
		echo "</div>";
	}
	
} // end class Forumview
/**@}*/?>