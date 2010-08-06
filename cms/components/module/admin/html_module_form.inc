<?php

Fakoli::using("module");
Fakoli::usingFile("framework/auto_form.inc");

Fakoli::assertRole("admin");

$menu_item = "Modules";
$module_id = checkNumeric($_GET["module_id"]);

$module = new Module();
$module->content_type = HTML;

$form = new AutoForm($module);
//$form->override("template", $label = "HTML", $renderer = "HTML");
$form->override("template", $label = "HTML", new HTMLFieldRenderer($form));

$form->required("title");
$form->hide("content_type","ord_by","query_constraint","num_items", "php_code_file", "menu_id", "menu_parameters", "group_id");
$form->alias("css_class", "CSS Class");
$form->allowDelete = true;
$redirect = "modules";
$form->button("Cancel", $redirect);

if ($method == "POST")
{
	if ($form->save())
	{
		redirect("modules");	
	}
}

if ($module_id)
{
	$module->load($module_id);
	$module->content_type = HTML;
	$title = "Edit Module: {$module->title}";
	$form->override("template", $label = "HTML", new HTMLFieldRenderer($form));
	$form->button("Run the Module", "module_preview?module_id=$module_id");
}
else
{
	$title = "Add a New HTML Module";
}

$script .= $form->writeScript();

require_once admin_begin_page;

$form->drawForm();

require_once admin_end_page;
?>
