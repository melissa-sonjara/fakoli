<?php
Fakoli::using("forum");

$constraint = checkRole("admin") ? "ORDER BY title" : "WHERE published=1 ORDER BY title";

$forumSummaries = $site->ForumSummaries($constraint);

$forumListView = new ForumListView($forumSummaries, "forum_details", "forum_topic");
$forumListView->table->cssStyle="width: 100%; clear: both";

$script .= $forumListView->writeScript();

$forumListView->drawView();

if (checkRole("admin"))
{
?>
<br/>
<button onclick="go('/forum_form')" class="button">Create a new Discussion Forum</button>
<? 
}
elseif(checkRole('member'))
{
?>
<br/>
<button onclick="go('/forum_request_form')" class="button">Request a new Forum</button>
<?
}
?>