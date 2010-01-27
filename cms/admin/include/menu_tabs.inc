<?php
require_once realpath(dirname(__FILE__))."/../../../framework/tab_bar.inc";

function menuTabs($key)
{
	$tabs = array(	"Menu Details"	=>	"menu_form.php",
				    "Menu Items"	=>	"menu_items.php"
				 );
					
   
	$qs = ($key) ? "menu_id=$key" : "";
	return new TabBar("tabs", $tabs, $qs);
}
?>