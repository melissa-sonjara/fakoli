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

class ForumSearchResultsView
{
	var $messages;
	var $searchText;
	
	function __construct($messages, $searchText, $id = "forum_search_results")
	{
		$this->messages = $messages;
		$this->searchText = $searchText;
	}
	
	function writeScript()
	{
	}
	
	function drawView()
	{
		global $user; // User Credentials from session
	
		$first = true;
		$alt = false;
	
		$forum_id = checkNumeric($_GET["forum_id"]);
		$forum = new Forum($forum_id);
			
?>
<p><label>Search Results for</label> <?echo htmlSafe($this->searchText)?>
<a class='button' style='float: right' href='<?echo $forum->identifier?>'>&laquo; Back to Forum</a>
</p>
<?
		if (count($this->messages) > 0)
		{
			foreach($this->messages as $message)
			{
				$author = $message->Author();
	
				$userProfile = ForumManager::formatUserProfile($author);
				$class = $alt ? "message alt" : "message"
				?>
		<a name="message<?echo $message->message_id?>"></a>
		<div id="message<?echo $message->message_id?>" class="<?echo $class?>">
		 <div class="title">
	
		   <a class="button" style='float: right' href='forum_topic?forum_id=<?echo $message->forum_id?>&topic_id=<? echo $message->topic_id ?>'> View this topic </a>	

		  <strong><?echo $message->title?></strong><br/>
		  <span class="small">Posted by <?echo $userProfile?> at <?echo $message->date_posted?></span>
		 </div>
		 <div class="message_content">
		  <?echo $message->message?>
		 </div>
		 <?
				 	$attachments = $message->Attachments();
				 	$userMgr = new UserManager();
				 	$cl = $userMgr->getUserClass();
				 	$u = new $cl;
				 	$pk = $u->getPrimaryKey();
				 	
				 	if (count($attachments) > 0)
				 	{
				 		echo "<h4 style='margin-bottom: 4px'>Attachments</h4>";
				 		
				 		foreach($attachments as $attachment)
				 		{
				 			$icon = DocumentHandler::getDocIcon($attachment->filename);
	
				 			echo $this->formatAttachment($icon, $attachment);
			 			
							if ($author && $user && ($author->get($pk) == $user->get($pk)) || checkRole("admin,data"))
							{
	?>
	&nbsp;&nbsp;<a style="font-size: 10px" href="/action/forum/attachment_delete?forum_id=<?echo $this->topic->forum_id?>&message_id=<?echo $message->message_id?>&attachment_id=<?echo $attachment->attachment_id?>" onclick="return confirm('Are you sure you want to delete <?echo $attachment->filename?>?');">Delete this attachment</a>
	<?
							}	
	?>
		<br/>	
	<?
				 		}
				 	}
					
				 	$controls = array();
					if ($user && $author->get($pk) == $user->get($pk) || checkRole("admin,data,moderator"))
					{
						$controls[] = "<a href=\"forum_message_form?forum_id={$this->topic->forum_id}&topic_id={$this->topic->topic_id}&message_id={$message->message_id}\">Edit</a>";
					}
				
					if ($user && $author->get($pk) == $user->get($pk) || checkRole("admin,data,moderator"))
					{
						$controls[] = "<a href=\"/action/forum/message_delete?forum_id={$this->topic->forum_id}&topic_id={$this->topic->topic_id}&message_id={$message->message_id}\" onclick=\"return confirm('Are you sure you want to delete this message?');\">Delete</a>";
					}		
					
					if ($user)
					{
						$controls[] = "<a href=\"forum_message_form?forum_id={$this->topic->forum_id}&topic_id={$this->topic->topic_id}&parent_id={$message->message_id}\">Reply</a>";
					}
		 	
	?>
		<div class="message_controls"><? echo implode("&nbsp;|&nbsp;", $controls);?></div>
		</div>
	<?				
					$first = false;
					$alt = !$alt;
				}
			}
			else
			{
	?>
		<p><em>No messages have been posted for this topic.</em></p>
	<?
			}
		}
		
		function formatAttachment($icon, $attachment)
		{
			global $config;
			$dir = $config["uploaddir"];
			
			$link = "<img src=\"{$icon}\" alt=\"Icon\" style=\"display:inline-block;vertical-align:middle\"/>&nbsp;"; 
			$link .= "<a href='/action/attachment/download?attachment_id={$attachment->attachment_id}'>{$attachment->filename}
			</a>&nbsp;({$attachment->file_size})\n";
			
			return $link;
		}			
}

/**@}*/?>