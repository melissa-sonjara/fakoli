<?php

Fakoli::using("module", "menu", "field_renderers");
Fakoli::usingFile("framework/auto_form.inc");

$menu_item = "Modules";
$module_id = checkNumeric($_GET["module_id"]);
$module = new Module();

$module->filter = new InclusionFilter("module_id", "title", "content_type", "css_class", "menu_id", "menu_parameters", "global", "global_position");
$module->content_type = "Menu";

$form = new AutoForm($module);
$form->required("title");
$form->hide("content_type");
$form->alias("css_class", "CSS Class");

$form->allowDelete = true;
$redirect = "/admin/modules";
$form->button("Cancel", $redirect);

$menuSelect = new RelatedItemSelectFieldRenderer($form, "menu_id", "Menu", Menu, "ORDER BY name", "name", "menu_id");

if ($method == "POST")
{	
	if ($form->save())
	{
		redirect("/admin/modules");	
	}
}

if ($module_id)
{
	$module->load($module_id);
	$title = "Edit Module: {$module->title}";
	$form->button("Run the Module", "/admin/module_preview?module_id=$module_id");
}
else
{
	$title = "Add a New Menu Module";
}

//$script .= $form->writeScript();



$form->drawForm();


?>
