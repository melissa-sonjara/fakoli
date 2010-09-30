<?php


Fakoli::using("menu", "site", "role", "page", "field_renderers");
Fakoli::usingFile("framework/auto_form.inc");

require_once "include/menu_tabs.inc";

$menu_item = "Menu";

$menu_id = checkNumeric($_GET["menu_id"]);
$menu_item_id = checkNumeric($_GET["menu_item_id"]);

$parent_id = checkNumeric($_GET["parent_id"]);

$menuItem = new MenuItem();
if ($menu_item_id)
{
	$menuItem->load($menu_item_id);
}
else
{
	$menuItem->parent_id = $parent_id;
	$menuItem->menu_id = $menu_id;
}

$menu = $menuItem->Menu();
$site = $menu->Site();

$tabs = menuTabs($menu_id);
$tabs->page = "menu_items";

$form = new AutoForm($menuItem);

$form->required("title");
//$form->allowDelete = true;
$form->hide("parent_id", "menu_id", "page_id");
/*
$pageChoices = array("Content Pages" => array(), "Component Pages" => array());

$contentPages = Query::create(Page, "ORDER BY page_title")->execute();
foreach($contentPages as $page)
{
	$pageChoices["Content Pages"][$page->identifier] = $page->page_title;
}
$componentPages = Query::create(ComponentPage, "ORDER BY component, page_title")->execute();
foreach($componentPages as $page)
{
	$pageChoices["Component Pages"][$page->identifier] = "$page->component: $page->page_title";
}

$identifierSelect = new SelectFieldRenderer($form, "identifier", "Page", $pageChoices, null, true);
*/
$identifierSelect = new CMSItemSelectFieldRenderer($form, "identifier", "Page", null);

$roleList = new CheckListFieldRenderer($form, "role", "Role", SiteRole::getRolesArray());


if ($method == "POST")
{
	if ($_POST["reorder_subitems"])
	{
	   	$sort_nums = checkNumeric($_POST["m"]);
		
		foreach($sort_nums as $key => $sort_num)
		{
			$m = new MenuItem();
			$m->load($key);
			$m->sort_order = $sort_num;
			$m->save();
		}		
	}
	else 
	{
		// Set up the page template for pages added via "or add new"
		$pageTemplate = new Page();
		$pageTemplate->identifier = codify(strtolower($_POST['autoform_add_page_id']));
		$pageTemplate->author = "{$user->first_name} {$user->last_name}";
		$pageTemplate->created_date = now();
		$pageTemplate->site_id = $site->site_id;
		$pageTemplate->template = $site->default_template;
		
		$pageSelect->templateItem = $pageTemplate;
		
		$form->save();
		if ($form->deleted)
		{
			redirect("/admin/menu_items?menu_id={$menu->menu_id}");
		}
		else
		{
			if ($parent_id)
			{
				redirect("/admin/menu_item_form?menu_item_id={$parent_id}");
			}
			else
			{
				redirect("/admin/menu_items?menu_id={$form->data->menu_id}");
			}
		}
	}
}

if ($menu_item_id)
{
	$title = "Edit Menu Item Details for {$menuItem->title}";
	
	$submenus = $menuItem->Children();
	if(count($submenus) > 0) {
		$form->allowDelete = false;
		}
	else { 
		$form->allowDelete = true; 
	}
	
	$form->button("Move Menu Item", "/admin/menu_move_form?menu_item_id=$menu_item_id&parent_id=$parent_id");
	$form->button("Cancel", $redirect);
}
else
{
	$title = "Add a new Menu Item";
	$form->button("Cancel", $redirect);
}

$script .= $form->writeScript();



$tabs->writeHTML();
?>
<div id="form" style="clear:left;border:solid 1px #000; padding: 4px;">
<h4><?echo $title?></h4>
<div id="breadcrumbs"><? echo menuAdminBreadcrumbs($menuItem, $menu)?></div>
<?

$form->drawForm();

if ($menu_item_id)
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
  <td><a href="/admin/menu_item_form?menu_item_id=<?echo $m->menu_item_id?>"><?echo $m->title ?></a></td>
 </tr>
<?
		}
?>
</table>

<br/>

<a href="#" class="button" onclick="document.forms['menu_order_form'].submit();">Update Menu Order</a>&nbsp;&nbsp;<a href="menu_item_form?menu_id=<?echo $menu->menu_id?>&parent_id=<?echo $menu_item_id?>" class="button">Add a new Sub-item</a>

</form>
<?
	}
	else
	{
?><p><i>There are no sub-items associated with this menu item.</i></p>
<a href="/admin/menu_item_form?menu_id=<?echo $menu->menu_id?>&parent_id=<?echo $menu_item_id?>" class="button">Add a new Sub-item</a>
<?
	}
}
?>
</div>
<?

?>
