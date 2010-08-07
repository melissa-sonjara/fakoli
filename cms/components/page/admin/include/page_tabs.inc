<?php
Fakoli::usingFile("framework/tab_bar.inc");

function pageTabs($key)
{
	$tabs = array(	"Content"	=>	"/admin/page_form",
					"Modules"	=>	"/admin/page_modules"
					);

	$qs = ($key) ? "page_id=$key" : ""; 
	return new TabBar("tabs", $tabs, $qs);
}
?>