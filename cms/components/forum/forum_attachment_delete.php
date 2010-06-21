<?php
$page_role = "admin,member,data";

require_once "../../../include/config.inc";
require_once "../../datamodel/forum.inc";
require_once "../../datamodel/attachment.inc";

$forum_id = $_GET["forum_id"];
$message_id = $_GET["message_id"];
$attachment_id = $_GET["attachment_id"];

if (!$forum_id || !$message_id || !$attachment_id) redirect("/");

$forum = new Forum($forum_id);
$message = new ForumMessage($message_id);

if ($message->forum_id != $forum_id) redirect("/");

$attachment = new Attachment($attachment_id);

$jns = query(ForumMessageAttachmentXref, "WHERE message_id=$message_id AND attachment_id=$attachment_id");

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

redirect($_SERVER['HTTP_REFERER']);

?>