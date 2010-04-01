<?php
require_once "../../include/config.inc";

require_once "../datamodel/module.inc";
require_once "../datamodel/menus.inc";
require_once "../datamodel/group.inc";
require_once "../../include/permissions.inc";
require_once "../../framework/auto_form.inc";
require_once "../components/field_renderers/code_file_select_field_renderer.inc";

$menu_item = "Modules";
$module_id = checkNumeric($_GET["module_id"]);
$group = get_class(new Group());

$module = new Module();

$module->filter = new InclusionFilter("module_id", "title", "content_type", "css_class", "menu_id", "menu_parameters", "global", "global_position");
$module->content_type = "Menu";

$form = new AutoForm($module);
$form->required("title");
$form->hide("content_type");
$form->alias("css_class", "CSS Class");

$form->allowDelete = true;
$redirect = "modules.php";
$form->button("Cancel", $redirect);

$menuSelect = new RelatedItemSelectFieldRenderer($form, "menu_id", "Menu", Menu, "ORDER BY name", "name", "menu_id");

if ($method == "POST")
{	
	if ($form->save())
	{
		redirect("modules.php");	
	}
}

if ($module_id)
{
	$module->load($module_id);
	$title = "Edit Module: {$module->title}";
	$form->button("Run the Module", "module_preview.php?module_id=$module_id");
}
else
{
	$title = "Add a New Menu Module";
}

//$script .= $form->writeScript();

require_once admin_begin_page;

$form->drawForm();

require_once admin_end_page;
?>
