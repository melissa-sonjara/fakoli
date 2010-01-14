<?php
require_once realpath(dirname(__FILE__))."/../../framework/tab_bar.inc";

function menuTabs($key)
{
	$tabs = array(	"Menu Details"	=>	"menus_form.php",
					    "Menu Order"	=>	"menu_reorder_form.php"
					);
					
   
	$qs = ($key) ? "parent_id=$key" : "";
	return new TabBar("tabs", $tabs, $qs);
}
?>