<?php
Fakoli::usingFile("framework/tab_bar.inc");

function blogTabs($blog_id)
{
	$tabs = array(	"Blog Definition"	=>	"/admin/blog_form",
					"Articles"			=>	"/admin/blog_articles"
					);

	$bar = new TabBar("tabs", $tabs, "blog_id=$blog_id");
	
	return $bar;
}
?>