<?php

Fakoli::using("forum", "attachment", "user");

Fakoli::usingFeature("auto_form");

$forum_id = checkNumeric($_GET["forum_id"]);
$parent_id = checkNumeric($_GET["parent_id"]);
$message_id = checkNumeric($_GET["message_id"]);

$forum = new Forum($forum_id);

if (!$forum) redirect("/index");

$mgr = new UserManager();

if ($message_id)
{
	$message = new ForumMessage($message_id);
	$message->filter = new ExclusionFilter("date_posted");
}
else
{
	$message = new ForumMessage();
	if ($parent_id)
	{
		$parent = new ForumMessage($parent_id);
		$message->topic_id = $parent->topic_id;
		$message->title = "Re: {$parent->title}";
		$message->title = preg_replace("/^(Re:\s+)+/", "Re: ", $message->title);
		$page->page_title = "Reply To: {$parent->title}";
		$parentAuthor = $parent->Author();
		$message->message = "<br/><div class='quote'><strong>{$parentAuthor->getFullName()} wrote on {$parent->date_posted}:</strong><br/>{$parent->message}</div><br/><br/>";
	}
	else
	{
		$page->page_title = "Create a Topic for {$forum->title}";

	}

	$_POST['author_id'] = $user->user_id;
	$_POST['forum_id'] = $forum_id;
	$_POST['parent_id'] = $parent_id;
}


$form = new AutoForm($message);
$form->button("Cancel", "/forum_details?forum_id=$forum_id");

$form->hide("parent_id", "topic_id", "author_id", "forum_id", "date_posted", "deleted");

$attachmentHandler = new AttachmentFieldRenderer($form, "attachments", ForumMessageAttachmentXref);

if ($method == "POST")
{
	$_POST['date_posted'] = now();
	
	if ($message_id)
	{
		// Editing the message - add an edit notification to the end of the message
		$_POST["message"] .= "<p><em>Edited by {$user->first_name} {$user->last_name} on ".now()."</em></p>";
	}
	
	if ($form->save())
	{
		if (!$parent_id && !$message_id)
		{
			// Creating a new topic
			$topic = new ForumTopic();
			$topic->forum_id = $forum_id;
			$topic->message_id = $form->data->message_id;
			$topic->date_created = $form->data->date_posted;
			$topic->save();
			
			$form->data->filter = new InclusionFilter("message_id", "topic_id");
			$form->data->topic_id = $topic->topic_id;
			$form->data->save();
			
			redirect("/forum_details?forum_id={$forum_id}");
		}
		else
		{
			redirect("/forum_topic?forum_id={$forum_id}&topic_id={$message->topic_id}");
		}
	}
}

$script .= $form->writeScript();

$form->drawForm();
?>