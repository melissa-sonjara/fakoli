<?php

Fakoli::using("blog");
Fakoli::usingFeature("data_view");

$title = "Blog Subscriber List";

$blogs = query (Blog, " WHERE published=1");

foreach ($blogs as $blog)
{
	$blog_id = $blog->blog_id;
	$blog_title = $blog->title;
	
	echo ("<h3>" . $blog_title . "</h3>");
	
	$blogSubscriberListView = new BlogSubscriberListView($blog_id);
	$blogSubscriberListView->drawView();	
	
}

?>