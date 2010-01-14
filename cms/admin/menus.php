<?php

require_once "../../include/config.inc";

require_once "../datamodel/menus.inc";
require_once "../../include/permissions.inc";

require_once "../../framework/tree.inc";

$menu_item = "Menu";

$title = "Manage Menu and Menu Items";
$menu_id = checkNumeric($_GET["menu_id"]);
$parent_id = checkNumeric($_GET["parent_id"]);

$menus = query(Menus, "ORDER BY sort_order");

$menuTree = new TreeControl("menu_tree");

$menuTree->height = 300;


if (count($menus) > 0)
{
	foreach ($menus as $m) {
	    $menuNode = new TreeNode("menu_{$m->menu_id}", $m->title, null, false, "tree_node_closed", "tree_node_open", "menus_form.php?menu_id={$m->menu_id}&parent_id={$m->parent_id}");
	    //$help_pageNode->leafStyle = "file_node_leaf";
	    $tmp = Array( 
	                       'parent' => $m->parent_id, 
	                       'node' => $menuNode);
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


$script = $menuTree->writeScript();
$script .= "<link type='text/css' rel='stylesheet' href='/css/tree.css'/>";

require_once admin_begin_page;

?>
<table border="0">
 <tr>
  <td colspan="2">
<?
$menuTree->writeHTML();
?>
  </td>
 </tr>
 <tr>
  <td colspan="2">&nbsp;</td>
 </tr>
 <tr>
  <td align="left"><input type="button" class="button" value=" Add a New Menu" onclick="go('menus_form.php?parent_id=0');"></td>
  
 </tr>
</table>
<?


require_once admin_end_page;

?>
