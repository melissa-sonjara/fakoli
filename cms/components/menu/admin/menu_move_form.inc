<?php
/**************************************************************
 *
 * Description: Move a page within the menu
 * hierarchy.
 * 
 * Reshma Joshi for Sonjara, Inc.
 * 
 * modified, Janice Gallant for Sonjara, Inc., adapted
 * from help page code.
 * 
 * 9/30/2010
 * 
 ****************************************************************/


Fakoli::using("menu", "site", "role", "page", "field_renderers");
Fakoli::usingFile("framework/auto_form.inc","framework/tree.inc");

$menu_item_id = checkNumeric($_GET["menu_item_id"]);
$parent_id = checkNumeric($_GET["parent_id"]);

$menuItem = new MenuItem();

if ($menu_item_id)
{
	$menuItem->load($menu_item_id);
	$title = "Move Menu Item: {$menuItem->title}";
}
else
	$title = "Add a new Menu Item";

$form = new AutoForm($menuItem);

$form->hide("title", "page_id", "sort_order", "identifier", "menu_id", "url");
$form->alias("parent_id", "Move Menu Item To: ");
$form->onSaveComplete = UpdateRolesToMatchNewParent;

$redirect = "/admin/menu_item_form?menu_item_id={$menu_item_id}&parent_id={$parent_id}";
$form->button("Cancel", $redirect);

if($parent_id)
	$form->submitLabel = "Move Page";

$menuItems = query(MenuItem, "ORDER BY sort_order");

$menuItemTree = new TreeControl("parent_id");
$menuItemTree->width = 500;
$menuItemTree->height = 280;
$menuItemTree->selectMode = "single";

if ($method == "POST") 
{
 	if($form->save())
		redirect($redirect);
}

if (count($menuItems) > 0)
{   
	$root_checked = ($parent_id == 0);
	$emptyNode = new TreeNode("emptyNode", "Top Level", "0", $root_checked, "tree_node_closed", "tree_node_open");
	$menuItemTree->add($emptyNode);
	
	foreach ($menuItems as $menuItem)
	{	
		$checked = ($menuItem->menu_item_id == $parentkey);
        
	    if( ($menuItem->menu_item_id == $menu_item_id ) || ($menuItem->parent_id == $menu_item_id) ) 
	    {
	    	$node = new TreeNode("menuItemPage_{$menuItem->menu_item_id}", $menuItem->title, "", false, "tree_node_closed", "tree_node_open");
	    }
		else 
		{
		     $node = new TreeNode("menuItemPage_{$menuItem->menu_item_id}", $menuItem->title, $menuItem->menu_item_id, $checked, "tree_node_closed", "tree_node_open");
	    }
	   
	    $tmp = Array( 
	                       'parent' => $menuItem->parent_id, 
	                       'node' => $node);
	    $displays[$menuItem->menu_item_id] = $tmp;
	}	

	foreach ($displays as $display) 
	{	
		if (empty($display['parent'])) 
		{
			$menuItemTree->add($display['node']);
		}
		else 
		{
			$parentNode = $displays[ $display['parent'] ]['node'];
			$parentNode->add($display['node']);
		}
	}
}

$treeSelect = new TreeSelectFieldRenderer($form, "parent_id", $menuItemTree);

echo 
$script = $form->writeScript();
$script .= "<link type='text/css' rel='stylesheet' href='/css/tree.css'/>";
?>
<h4>Note: when you move a page, that page's child pages will move with it. The page's roles will change to match its new position: any roles selected for this page that are not also selected for its new parent page will be removed.</h4>
<?
$form->drawForm();

/*
 * We need to update this page's and its child pages to have the 
 * same roles as the new parent page. If the parent page does not
 * have one or more roles that this page has, then we must
 * remove that role from this page and its child pages.
 * 
 */
function UpdateRolesToMatchNewParent($form)
{
	$menuItem = $form->data;
	$parent = $menuItem->Parent();
	
	$tobeRemoved = getRoleDiff($menuItem->role, $parent->role);
	
	updateChildRoles($menuItem, $tobeRemoved);
}


/*
 * Get the roles that are in the first string but not in the 2nd.
 */
function getRoleDiff($string1, $string2)
{
	$in1Not2 = array();
	
	if($string1 != $string2)
	{
		$roles1 = ($string1) ? explode(",", $string1) : array();
		$roles2 = ($string2) ? explode(",", $string2) : array();

		// get roles that are in roles1 but not in roles2
		$in1Not2 = array_diff($roles1, $roles2);
	}
	
	return $in1Not2;
}
	

function updateChildRoles($menuItem, $tobeRemovedRoles)
{
	if(count($tobeRemovedRoles) > 0)
	{
		$childPages = array();
		$childPages = getChildMenuItems($menuItem, $childPages);
		foreach($tobeRemovedRoles as $oldRole)
		{
			foreach($childPages as $child)
			{
				$child->role = preg_replace("/$oldRole,*/", "", $child->role);
				$child->filter = new InclusionFilter("role");
					
				// remove comma as first char, if it exists
				$child->role = preg_replace("/^,*/", "", $child->role);
				$child->save();
			}
		}
	}	
}


/*
 * Recursively create an array of all child pages of $menuItem down to the
 * leaf page.
 */
function getChildMenuItems($menuItem, &$childPages)
{
    $children = $menuItem->Children();
 
    if($children) 
    {
		foreach($children as $child) 
		{
			array_push($childPages, $child);
			getChildPages($child, $childPages);
        }
    }

    return $childPages;
}
?>