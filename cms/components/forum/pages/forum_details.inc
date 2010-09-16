<?php

Fakoli::using("forum");

$forum_id = checkNumeric($_GET["forum_id"]);

$forum = new Forum($forum_id);

$page->page_title = $forum->title;

$view = new ForumDetailsView($forum, "forum_topic");
$view->table->cssStyle = "width: 700px; clear: both";

$script .= $view->writeScript();
?>
<div><?echo $forum->description?></div>
<?
$view->drawView();
?>
<br/>
<div style="width: 100%"><div style="float: right">
<a class="button" href="/forum_message_form?forum_id=<?echo $forum_id?>">Create a Topic</a>
<?
if (checkRole("admin"))
{
?>
&nbsp;&nbsp;<a class="button" href="/forum_form?forum_id=<?echo $forum_id?>">Edit this Discussion Forum</a>
<?
}
?>
</div>
<a class="button" href="/forum_list">&laquo; Back to Forum List</a>
</div>