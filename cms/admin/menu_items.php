<?php

require_once "../../include/config.inc";

require_once "../datamodel/menus.inc";
require_once "../../include/permissions.inc";
require_once "../../framework/tree.inc";
require_once "include/menu_tabs.inc";

$menu_item = "Menus";

$menu_id = checkNumeric($_GET["menu_id"]);

if (!$menu_id) redirect('/index.php');

$menu = new Menu($menu_id);

$menuItems = $menu->MenuItems("ORDER BY parent_id, sort_order");

$menuTree = new TreeControl("menu_tree");

$menuTree->height = 300;


$title = "Manage Menu Items for {$menu->name}";

$displays = array();


$rootNode = new TreeNode("menu", $menu->name);
$menuTree->add($rootNode);

$displays[0] = Array('parent' => 0, 'node' => $rootNode);

if (count($menus) > 0)
{
	foreach ($menus as $m) {
	    $menuNode = new TreeNode("menu_item_{$m->menu_item_id}", $m->title, null, false, "tree_node_closed", "tree_node_open", "menu_item_form.php?menu_item_id={$m->menu_item_id}&parent_id={$m->parent_id}");
	    //$help_pageNode->leafStyle = "file_node_leaf";
	    $tmp = Array('parent'	 => $m->parent_id, 
	                 'node'		 => $menuNode);
	    $displays[$m->menu_id] = $tmp;
	}	

	foreach ($displays as $display) {
		
		if (empty($display['parent'])) {
			$menuTree->add($display['node']);
		}
		else {
			$parentNode = $displays[ $display['parent'] ]['node'];
			if ($parentNode)
			{
				$parentNode->add($display['node']);
			}
		}
	}
}

$tabs = menuTabs($menu_id);

$script = $menuTree->writeScript();
$script .= "<link type='text/css' rel='stylesheet' href='/css/tree.css'/>";

require_once admin_begin_page;

$tabs->writeHTML();
?>
<div id="form" style="clear:left;border:solid 1px #000; padding: 4px;">
<table border="0">
 <tr>
  <td colspan="2">
  <h4>Menu Items</h4>
<?
$menuTree->writeHTML();
?>
  </td>
 </tr>
 <tr>
  <td colspan="2">&nbsp;</td>
 </tr>
 <tr>
  <td align="left"><input type="button" class="button" value=" Add a New Menu Item" onclick="go('menu_item_form.php?menu_id=<?echo $menu->menu_id?>&parent_id=0');"></td>
  
 </tr>
</table>
</div>
<?
require_once admin_end_page;
?>
