<?php
require_once "../../include/config.inc";

require_once "../datamodel/module.inc";
require_once "../datamodel/news_item.inc";
require_once "../datamodel/group.inc";
require_once "../datamodel/publication.inc";
require_once "../datamodel/page.inc";
require_once "../../include/permissions.inc";
require_once "../../framework/auto_form.inc";

$menu_item = "Modules";
$module_id = checkNumeric($_GET["module_id"]);
$news = get_class(new NewsItem());
$group = get_class(new Group());
$publication = get_class(new Publication());

$module = new Module();
$module->content_type = HTML;

$form = new AutoForm($module);
//$form->override("template", $label = "HTML", $renderer = "HTML");
$form->override("template", $label = "HTML", new HTMLFieldRenderer($form));

$form->required("title");
$form->hide("content_type","ord_by","query_constraint","num_items", "php_code_file");
$form->alias("css_class", "CSS Class");
$form->allowDelete = true;
$redirect = "modules.php";
$form->button("Cancel", $redirect);

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
	$module->content_type = HTML;
	$title = "Edit Module: {$module->title}";
	$form->override("template", $label = "HTML", new HTMLFieldRenderer($form));
	$form->button("Run the Module", "module_preview.php?module_id=$module_id");
}
else
{
	$title = "Add a New HTML Module";
}

//$contentSelect = new SelectFieldRenderer($form, "content_type", "Module Type", $mod_type);

$ord_by_Select = new SelectFieldRenderer($form, "ord_by", "Order by", $ord_by);


$grpSelect = new RelatedItemSelectFieldRenderer($form, "group_id", "Group", Group, "ORDER BY title", "title");


$script .= $form->writeScript();

require_once admin_begin_page;

$form->drawForm();

require_once admin_end_page;
?>
