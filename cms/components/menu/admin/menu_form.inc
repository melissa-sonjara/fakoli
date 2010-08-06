<?php


Fakoli::using("menu");
Fakoli::using("site");

Fakoli::assertRole("admin");

Fakoli::usingFile("framework/auto_form.inc");

require_once "include/menu_tabs.inc";

$menu_item = "Menus";

$menu_id = checkNumeric($_GET["menu_id"]);

$menu = new Menu();

$tabs = menuTabs($menu_id);

$form = new AutoForm($menu);

$form->required("title");
$form->allowDelete = true;
$redirect = "menus.php";
$form->button("Cancel", $redirect);
$siteSelect = new RelatedItemSelectFieldRenderer($form, "site_id", "Site", Site, "order by site_name", "site_name", "site_id");
if ($method == "POST")
{

	if ($form->save())
	{
		redirect("menus.php");	
	}
}

if ($menu_id)
{
	$menu->load($menu_id);
	$title = "Edit Menu Details for {$menu->name}";
}
else
{
	$title = "Add a New Menu";
}

$script .= $form->writeScript();

require_once admin_begin_page;

$tabs->writeHTML();
?>
<div id="form" style="clear:left;border:solid 1px #000; padding: 4px;">
<?
$form->drawForm();
?>
</div>
<?
require_once admin_end_page;
?>