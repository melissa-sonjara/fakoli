<?php
require_once realpath(dirname(__FILE__))."/../../../framework/tab_bar.inc";

function pageTabs($key)
{
	$tabs = array(	"Content"	=>	"page_form.php",
					"Modules"	=>	"page_modules.php"
					);

	$qs = ($key) ? "page_id=$key" : ""; 
	return new TabBar("tabs", $tabs, $qs);
}
?>