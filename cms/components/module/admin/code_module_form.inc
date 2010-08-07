<?php

Fakoli::using("module", "field_renderers");

Fakoli::usingFile("framework/auto_form.inc");

Fakoli::assertRole("admin");

$menu_item = "Modules";
$module_id = checkNumeric($_GET["module_id"]);

$module = new Module();

$module->filter = new InclusionFilter("module_id", "title", "php_code_file", "content_type", "css_class", "global", "global_position");
$module->content_type = "Code";

$form = new AutoForm($module);
$form->required("title");
$form->hide("content_type");
$form->alias("css_class", "CSS Class");

$form->allowDelete = true;
$redirect = "/admin/modules";
$form->button("Cancel", $redirect);

$codeSelect = new CodeFileSelectFieldRenderer($form, "php_code_file", "PHP Code File", "modules");

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
	$title = "Edit Module: {$module->title}";
	$form->button("Run the Module", "module_preview?module_id=$module_id");
}
else
{
	$title = "Add a New Code Module";
}

//$script .= $form->writeScript();



$form->drawForm();


?>
