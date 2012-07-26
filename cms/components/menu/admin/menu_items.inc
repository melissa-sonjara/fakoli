<?php
/**************************************************************

 Copyright (c) 2010 Sonjara, Inc

 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:

 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.

 Except as contained in this notice, the name(s) of the above 
 copyright holders shall not be used in advertising or otherwise 
 to promote the sale, use or other dealings in this Software 
 without prior written authorization.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.

*****************************************************************/

Fakoli::using("menu");

Fakoli::assertRole("admin");

Fakoli::usingFile("framework/tree.inc");

require_once "include/menu_tabs.inc";

$menu_item = "Menus";

$menu_id = checkNumeric($_GET["menu_id"]);

if (!$menu_id) redirect('/index');

$menu = new Menu($menu_id);

$menuItems = $menu->MenuItems("ORDER BY parent_id, sort_order, title");

$menuTree = new TreeControl("menu_tree");

$menuTree->height = 300;


$title = "Manage Menu Items for {$menu->name}";

$displays = array();


$rootNode = new TreeNode("menuTree", $menu->name);
$menuTree->add($rootNode);

if (count($menuItems) > 0)
{
	foreach ($menuItems as $m) {
	    $menuNode = new TreeNode("menu_item_{$m->menu_item_id}", $m->title, null, false, "tree_node_closed", "tree_node_open", "/admin/menu_item_form?menu_item_id={$m->menu_item_id}&parent_id={$m->parent_id}");
	    //$help_pageNode->leafStyle = "file_node_leaf";
	    $tmp = Array('parent'	 => $m->parent_id, 
	                 'node'		 => $menuNode);
	    $displays[$m->menu_item_id] = $tmp;
	}	

	foreach ($displays as $display) {
		
		if (empty($display['parent'])) {
			$rootNode->add($display['node']);
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
$tabs->page = "menu_items";

$script = $menuTree->writeScript();

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
  <td align="left"><input type="button" class="button" value=" Add a New Menu Item" onclick="go('/admin/menu_item_form?menu_id=<?echo $menu->menu_id?>&parent_id=0');"></td>
  
 </tr>
</table>
</div>
<?

?>
