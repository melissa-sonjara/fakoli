<?php

Fakoli::usingFile("framework/tab_bar.inc");

function menuTabs($key)
{
	$tabs = array(	"Menu Details"	=>	"menu_form",
				    "Menu Items"	=>	"menu_items"
				 );
					
   
	$qs = ($key) ? "menu_id=$key" : "";
	return new TabBar("tabs", $tabs, $qs);
}

function menuAdminBreadcrumbs($menuItem, $menu)
{
	$breadcrumb = array();
	if ($menuItem)
	{
		$breadcrumb[] = ($menuItem->title) ? $menuItem->title : "New Node";
		
		
		while($menuItem->parent_id)
		{
			$menuItem = $menuItem->Parent();
			
			$breadcrumb[] = "<a href='menu_item_form?menu_item_id={$menuItem->menu_item_id}'>".(($menuItem->title) ? $menuItem->title : "New Node")."</a>";
		}
	}
		
	$breadcrumb[] = "<a href='menu_form?menu_id={$menu->menu_id}'>{$menu->name}</a>";
	$breadcrumb = array_reverse($breadcrumb);
	
	return implode("&nbsp;&raquo;&nbsp;", $breadcrumb);
}
?>