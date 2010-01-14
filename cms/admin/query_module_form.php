<?php
require_once "../../include/config.inc";

require_once "../datamodel/module.inc";
require_once "../datamodel/news_item.inc";
require_once "../datamodel/grp.inc";
require_once "../datamodel/publication.inc";
require_once "../datamodel/page.inc";
require_once "../../include/permissions.inc";
require_once "../../framework/auto_form.inc";

$menu_item = "Modules";
$module_id = checkNumeric($_GET["module_id"]);
$news = get_class(new NewsItem());
$group = get_class(new Grp());
$publication = get_class(new Publication());

$module = new Module();

$form = new AutoForm($module);
$form->required("title");
$form->hide("php_code_file");

$form->allowDelete = true;

$form->getRenderer("template")->rows = 10;

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
	$title = "Add a Query New Module";
}

$contentSelect = new SelectFieldRenderer($form, "content_type", "Module Type", $mod_type);

$ord_by_Select = new SelectFieldRenderer($form, "ord_by", "Order by", $ord_by);


$grpSelect = new RelatedItemSelectFieldRenderer($form, "grp_id", "Group", Grp, "ORDER BY title", "title");


//$script .= $form->writeScript();

require_once admin_begin_page;

$form->drawForm();

require_once admin_end_page;
?>
