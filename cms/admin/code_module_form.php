<?php
require_once "../../include/config.inc";

require_once "../datamodel/module.inc";
require_once "../datamodel/news_item.inc";
require_once "../datamodel/group.inc";
require_once "../datamodel/publication.inc";
require_once "../datamodel/page.inc";
require_once "../../include/permissions.inc";
require_once "../../framework/auto_form.inc";
require_once "../components/field_renderers/code_file_select_field_renderer.inc";

$menu_item = "Modules";
$module_id = checkNumeric($_GET["module_id"]);
$news = get_class(new NewsItem());
$group = get_class(new Group());
$publication = get_class(new Publication());

$module = new Module();

$module->filter = new InclusionFilter("module_id", "title", "php_code_file", "content_type");
$module->content_type = "Code";

$form = new AutoForm($module);
$form->required("title");
$form->hide("content_type");

$form->allowDelete = true;

$form->getRenderer("template")->rows = 10;

$codeSelect = new CodeFileSelectFieldRenderer($form, "php_code_file", "PHP Code File", "modules");

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
	$title = "Add a New Code Module";
}

//$script .= $form->writeScript();

require_once admin_begin_page;

$form->drawForm();

require_once admin_end_page;
?>
