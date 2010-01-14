<?php

require_once "../../include/config.inc";

require_once "../datamodel/menus.inc";
require_once "../datamodel/page.inc";
require_once "../../include/permissions.inc";
require_once "../../framework/auto_form.inc";

$menu_item = "Menu";

$menu_id = checkNumeric($_GET["menu_id"]);
$parent_id = checkNumeric($_GET["parent_id"]);

$menuItem = new Menus();
if ($menu_id)
{
	$menuItem->load($menu_id);
}
else
{
	$menuItem->parent_id = $parent_id;
}
$form = new AutoForm($menuItem);
$redirect = "menus.php";


$form->required("title");
//$form->allowDelete = true;
$form->hide("parent_id");
$pageSelect = new RelatedItemSelectFieldRenderer($form, "page_id", "Page", Page, "ORDER BY page_title", "page_title", "page_id", true, true);

$menuOptions = array(

	krc	=>	"KRC",
	ncuih 	=>	"NCUIH"
);

$menu_type = new SelectFieldRenderer($form, "menu_type", "Menu Type", $menuOptions); 

if ($method == "POST")
{
	if ($_POST["reorder_subitems"])
	{
	   	$sort_nums = checkNumeric($_POST["m"]);
		
		foreach($sort_nums as $key => $sort_num)
		{
			$m = new Menus();
			$m->load($key);
			$m->sort_order = $sort_num;
			$m->save();
		}		
	}
	else 
	{
		$form->save();
		if ($form->deleted)
		{
			redirect("menus.php");
		}
		else
		{
			redirect("?menu_id={$form->data->menu_id}&parent_id={$parent_id}");
		}
	}
}

if ($menu_id)
{
	$title = "Edit Menu Details for {$menuItem->title}";
	
	$submenus = query(Menus, "WHERE parent_id=$menu_id ORDER BY sort_order");
	if(count($submenus) > 0) {
		$form->allowDelete = false;
		}
	else { 
		$form->allowDelete = true; 
	}
	
	$form->button("Move Menu", "menu_move_form.php?menu_id=$menu_id&parent_id=$parent_id");
	$form->button("Add Menu Item", "menus_form.php?parent_id=$menu_id");
	$form->button("Cancel", $redirect);
}
else
{
	 if ($parent_id ==0) {
	
		$title = "Add a new Menu";
		$form->button("Cancel", $redirect);
	 }
	 else {
	 	$title = "Add a Menu Item" 	;
	 	$form->submitLabel = "Add Menu Item";
	 	$form->button("Cancel", $redirect);
	 }	 
}

$script .= $form->writeScript();

require_once admin_begin_page;

$form->drawForm();

if ($menu_id)
{
?>
<h4>Sub-items</h4>
<?
	if ($submenus)
	{
?>

<form name='menu_order_form' method="POST" action="">
<input type="hidden" name="reorder_subitems" value="1"/>
<table class="list" style='width: 90%; margin-left: 4%'>
 <tr>
  <th>Sort Order</th>
  <th>Menu</th>
 </tr>
<?
		foreach($submenus as $m)
		{
?>
 <tr>
  <td style="text-align: center; vertical-align: middle; width: 80px"><input type="text" style="width: 60px" name="m[<?echo $m->menu_id ?>]" value="<?echo $m->sort_order ?>"/></td>
  <td><a href="menus_form.php?menu_id=<?echo $m->menu_id?>&parent_id=<?echo $m->parent_id?>"><?echo $m->title ?></a></td>
 </tr>
<?
		}
?>
</table>

<br/>

<a href="#" class="button" onclick="document.forms['menu_order_form'].submit();">Update Menu Order</a>&nbsp;&nbsp;<a href="menus_form.php?parent_id=<?echo $menu_id?>" class="button">Add a new Sub-item</a>

</form>
<?
	}
	else
	{
?><p><i>There are no sub-items associated with this menu item.</i></p>
<a href="menus_form.php?parent_id=<?echo $menu_id?>" class="button">Add a new Sub-item</a>
<?
	}
}

require_once admin_end_page;
?>
