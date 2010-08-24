<?php
Fakoli::usingFile("framework/tab_bar.inc");

function componentListTabs()
{
	$tabs = array(	"Components"		=>	"/admin/components",
					"Component Pages"	=>	"/admin/component_pages"
					);

	$bar = new TabBar("tabs", $tabs, "");
	$bar->useQueryString = false;
	
	return $bar;
}

function componentPageTabs($key)
{
	$tabs = array(	"Definition"	=>	"/admin/component_page_form",
					"Modules"		=>	"/admin/component_page_modules");
	
	$qs = ($key) ? "component_page_id=$key" : ""; 
	$bar = new TabBar("tabs", $tabs, $qs);
	
	return $bar;
}

?>