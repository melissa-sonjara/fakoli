<?php

Fakoli::using("blog");
Fakoli::usingFeature("data_view", "data_tabs");

$menu_item = "Blog Subscribers";

$blog_id = checkNumeric($_GET["blog_id"]);

$blogs = query(Blog, " WHERE published=1 ORDER BY title");

if (!count($blogs))
{
	echo "<p><em>There are no published blogs.</em></p>";
	return;
}

if (!$blog_id) $blog_id = $blogs[0]->blog_id;

$tabs = new DataItemTabBar("tabs", $blogs);

$blogSubscriberListView = new BlogSubscriberListView($blog_id);

$script .= $tabs->writeScript();

$tabs->writeHTML();
?>
<div id="tab_border">
<?
$blogSubscriberListView->drawView();
?>
</div>
