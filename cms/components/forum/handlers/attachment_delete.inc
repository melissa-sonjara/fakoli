<?php
Fakoli::using("forum", "attachment");

$forum_id	   = checkNumeric($_GET["forum_id"]);
$message_id	   = checkNumeric($_GET["message_id"]);
$attachment_id = checkNumeric($_GET["attachment_id"]);

if (!$forum_id || !$message_id || !$attachment_id) redirect("/");

$forum = new Forum($forum_id);
$message = new ForumMessage($message_id);

if ($message->forum_id != $forum_id) redirect("/");


if ($message->author_id == $user->get($user->primary_key) || checkRole("admin"))
{
	$attachment = new Attachment($attachment_id);
	
	$jns = Query::create(ForumMessageAttachmentXref, "WHERE message_id=:message_id AND attachment_id=:attachment_id")
				->bind(":message_id", $message_id, ":attachment_id", $attachment_id)
				->execute();
	
	if (count($jns) > 0)
	{
		foreach($jns as $jn)
		{
			$jn->delete();
		}
	}
	
	$filepath = "{$config['uploadbase']}/{$attachment->local_filename}";
	if (file_exists($filepath)) unlink($filepath);
	$attachment->delete();
}

redirect($_SERVER['HTTP_REFERER']);

?>