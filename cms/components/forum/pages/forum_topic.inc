<?php

Fakoli::using("forum", "attachment");

$forum_id = checkNumeric($_GET["forum_id"]);
$topic_id = checkNumeric($_GET["topic_id"]);

if (!$topic_id) redirect("/");

$topic = new ForumTopic($topic_id);
$message = $topic->FirstPost();
$page->page_title = $message->title;

$view = new ForumTopicView($topic);

$view->drawView();
?>
	<button style="float: left" class="button" onclick="go('/forum_details?forum_id=<?echo $topic->forum_id?>');"> &laquo; Back to Topic List</button>	
	<button style="float: left; margin-left: 425px" class="button" onclick="go('forum_message_form?forum_id=<?echo $topic->forum_id?>&parent_id=<?echo $topic->message_id?>')"> Reply to this topic </button>
<?
