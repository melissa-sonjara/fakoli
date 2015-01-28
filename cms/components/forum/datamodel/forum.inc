<?php
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

Fakoli::usingFile("framework/join.inc");

class Forum extends DataItem
{
	var $primary_key = "forum_id";
	var $table = "forum";
	var $default_format = "{title}";
	
	// Fields
	
	var $fields = array("forum_id"		=>	Number,
						"identifier"	=>	String,
						"title"			=>	String,
						"teaser"		=>	Text,
						"description"	=>	HTML,
						"created_date"	=>	Date,
						"last_modified"	=>	Timestamp,
						"owner_id"		=>	Number,
						"published"		=>	Boolean,
						"sort_order"	=>	Number);
	
	// Relations
	
	var $relations = array(
			"Sites"			=>	Site,
			"Owner"			=>	"",
			"Messages"		=>	ForumMessage,
			"Topics"		=>	ForumTopic,
			"LastPost"		=>	ForumMessage,
			"Summary"		=>	ForumSummary,
			"Subscriptions"	=>	ForumSubscription,
			"UserSubscriptions"	=>	ForumSubscription,
						);
	
	function Sites($constraint = "")
	{
		return $this->crossReference(Site, ForumSiteXref, $constraint);
	}
	
	function Owner()
	{
		$mgr = new UserManager();
		return $mgr->getUser($this->owner_id);
	}
	
	function Topics($constraint = "")
	{
		return $this->getRelatedList(ForumTopic, "", $constraint);
	}
	
	function Messages($constraint = "")
	{
		return $this->getRelatedList(ForumMessage, "", $constraint);
	}
	
	function LastPost()
	{
		$messages = $this->Messages("ORDER BY date_posted DESC LIMIT 1");
		return (count($messages) > 0) ? $messages[0] : null;
	}

	function Summary()
	{
		return $this->getRelated(ForumSummary);
	}
	
	function Subscriptions($constraint = "")
	{
		return $this->getRelatedList(ForumSubscription, "", $constraint);
	}
	
	/**
	 * Get the subscriptions to this forum for this user. There should at
	 * most one per user.
	 * 
	 * @return obj class ForumSubscription
	 */
	function UserSubscription()
	{
		global $user;
		$pk = $user->getPrimaryKey();
		
		$subscriptions = $this->Subscriptions("WHERE user_id={$user->$pk}");
		return count($subscriptions) ? $subscriptions[0] : null;
	}
	
	function allowDelete()
	{
		$count = queryValue(ForumMessage, "COUNT(1)", "WHERE forum_id={$this->forum_id} AND message != '' AND deleted=0");
		return ($count > 0) ? false : true;
	}
	
	static function findByIdentifier($identifier)
	{
		return Query::create(Forum, "WHERE identifier=:identifier")
					->bind(":identifier", $identifier)
					->executeSingle();
	}
	
	function __construct($forum_id)
	{
		parent::__construct($forum_id);
		
		// Patch in the user class, since this can be overridden by the application
		$mgr = new UserManager();
		$this->relations["Owner"] = $mgr->getUserClass(); 
	}
}


class ForumSiteXref extends DataItem
{
	var	$primary_key = "forum_site_xref_id";
	var $table = "forum_site_xref";
	
	// Fields
	
	var $fields = array("forum_site_xref_id"	=>	Number,
						"forum_id"				=>	Number,
						"site_id"				=>	Number);
	
	// Relations
	
	var $relations = array(	"Forum"	=>	Forum,
							"Site"	=>	Site);
	
	function Forum()
	{
		return $this->getRelated(Forum);
	}
	
	function Site()
	{
		return $this->getRelated(Site);
	}
}

class ForumMessage extends DataItem
{
	var	$primary_key = "message_id";
	var $table = "forum_message";
	var	$default_format = "{title}";
	
	// Fields

	var $fields = array("message_id"		=>	Number,
						"title"				=>	String,
						"message"			=>	HTML,
						"parent_id"			=>	Number,
						"topic_id"			=>	Number,
						"author_id"			=>	Number,
						"date_posted"		=>	Date,
						"last_modified"		=>	Timestamp,
						"forum_id"			=>	Number,
						"deleted"			=>	Boolean);
	
	// Relations
	
	var $relations = array(	"Forum"			=>	Forum,
							"Parent"		=>	ForumMessage,
							"Topic"			=>	ForumTopic,
							"Author"		=>	"",
							"Attachments"	=>	Attachment);
	
	function Forum()
	{
		return $this->getRelated(Forum);
	}
	
	function Parent()
	{
		return $this->getRelated(ForumMessage, "parent_id");
	}
	
	function Topic()
	{
		return $this->getRelated(ForumTopic);
	}
	
	function Author()
	{
		$mgr = new UserManager();
		return $mgr->getUser($this->author_id);
	}
	
	function Attachments($constraint = "")
	{
		return $this->crossReference(Attachment, ForumMessageAttachmentXref, $constraint);
	}
}


class ForumMessageAttachmentXref extends DataItem
{
	var	$primary_key = "forum_message_attachment_xref_id";
	var	$table = "forum_message_attachment_xref";
	
	// Fields
	var $fields = array("forum_message_attachment_xref_id"	=>	Number,
						"message_id"						=>	Number,
						"attachment_id"						=>	Number);
	
	// Relations
	var $relations = array(	"Message"		=>	ForumMessage,
							"Attachment"	=>	Attachment);
	
	function Message()
	{
		return $this->getRelated(ForumMessage);
	}
	
	function Attachment()
	{
		return $this->getRelated(Attachment);
	}
}

	
class ForumTopic extends DataItem
{
	var	$primary_key = "topic_id";
	var	$table = "forum_topic";
	
	// Fields
	var $fields = array("topic_id"		=>	Number,
						"message_id"	=>	Number,
						"forum_id"		=>	Number,
						"views"			=>	Number,
						"replies"		=>	Number,
						"date_created"	=>	Date);

	// Relations
	var $relations = array(	"FirstPost"	=>	ForumMessage,
							"Forum"		=>	Forum,
							"LastPost"	=>	ForumMessage,
							"Messages"	=>	ForumMessage);
	
	function FirstPost()
	{
		return $this->getRelated(ForumMessage);
	}
	 
	function LastPost()
	{
		$messages = $this->Messages("WHERE deleted=false ORDER BY date_posted DESC LIMIT 1");
		return count($messages) > 0 ? $messages[0] : null;
	}
	
	function Forum()
	{
		return $this->getRelated(Forum);
	}
	
	function getTitle()
	{
		return $this->FirstPost()->title;
	}
	
	function Messages($constraint = "")
	{
		return $this->getRelatedList(ForumMessage, "", $constraint);
	}
	
	/**
	 * When counting forum message replies to a topic, exclude the forum message
	 * that has a parent id of 0, that is the topic message, the initial post
	 * to which others may reply.
	 */
	function countReplies()
	{
		return Query::create(ForumMessage, "WHERE topic_id=:topic_id and parent_id > 0 and deleted=0")
			->bind(":topic_id", $this->topic_id)
			->executeValue("COUNT(1)");
	}
}

	
class ForumSummary extends DataItem
{
	var	$primary_key = "forum_id";
	var	$table = "forum_summary";
		
	// Fields
	var $fields = array("forum_id"		=>	Number,
						"title"			=>	String,
						"teaser"		=>	String,
						"topics"		=>	Number,
						"posts"			=>	Number,
						"published"		=>	Boolean,
						"sort_order"	=>	Number);
	
	// Relations
	var $relations = array("Forum"		=>	Forum,
						   "LastPost"	=>	ForumMessage);

	function Forum()
	{
		return $this->getRelated(Forum);
	}
	
	function LastPost()
	{
		$messages = $this->getRelatedList(ForumMessage, "", "WHERE deleted!=1 ORDER BY date_posted DESC LIMIT 1");
		return (count($messages) > 0) ? $messages[0] : null;
	}
}
?>