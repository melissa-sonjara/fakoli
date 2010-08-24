<?php

Fakoli::using("forum");

$forum_id = checkNumeric($_GET["forum_id"]);

$forum = new Forum($forum_id);

$page->page_title = $forum->title;

$view = new ForumDetailsView($forum, "forum_topic");
$view->table->cssStyle = "width: 950px; clear: both";

$script .= $view->writeScript();
?>
<div><?echo $forum->description?></div>
<?
$view->drawView();
?>
<br/>
<div style="width: 100%"><div style="float: right">
<button onclick="go('/forum/forum_message_form?forum_id=<?echo $forum_id?>')">Create a Topic</button>
<?
if (checkRole("admin"))
{
?>
&nbsp;&nbsp;<button onclick="go('/forum/forum_form?forum_id=<?echo $forum_id?>')">Edit this Discussion Forum</button>
<?
}
?>
</div>
<button onclick="go('/forum_list');">&laquo; Back to Forum List</button>
</div>