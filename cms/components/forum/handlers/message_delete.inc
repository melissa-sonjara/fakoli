<?php
Fakoli::using("forum");

$forum_id   = checkNumeric($_GET["forum_id"]);
$message_id = checkNumeric($_GET["message_id"]);

if (!$forum_id || !$message_id) redirect("/");

$forum = new Forum($forum_id);
$message = new ForumMessage($message_id);

if ($message->author_id == $user->get($user->primary_key) || checkRole("admin"))
{
	$message->deleted = true;
	$message->filter = new InclusionFilter("message_id", "deleted");
	$message->save();
}

redirect($_SERVER['HTTP_REFERER']);

?>