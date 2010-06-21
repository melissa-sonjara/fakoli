<?php
$page_role = "admin,member,data";

require_once "../../../include/config.inc";
require_once "../../datamodel/forum.inc";

$forum_id = $_GET["forum_id"];
$message_id = $_GET["message_id"];

if (!$forum_id || !$message_id) redirect("/");

$forum = new Forum($forum_id);
$message = new ForumMessage($message_id);

if ($message->author_id == $user->site_user_id || checkRole("admin,data"))
{
	$message->deleted = true;
	$message->filter = new InclusionFilter("message_id", "deleted");
	$message->save();
}

redirect($_SERVER['HTTP_REFERER']);

?>