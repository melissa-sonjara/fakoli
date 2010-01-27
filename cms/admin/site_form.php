<?php
require_once "../../include/config.inc";

require_once "../datamodel/site.inc";
require_once "../../include/permissions.inc";
require_once "../../framework/auto_form.inc";
require_once "../components/field_renderers/template_select_field_renderer.inc";

$menu_item = "Sites";

$site_id = checkNumeric($_GET["site_id"]);

$site = new Site();

$form = new AutoForm($site);

$form->required("title");
$form->allowDelete = true;
$redirect = "sites.php";
$form->button("Cancel", $redirect);
$templateSelect = new TemplateSelectFieldRenderer($form, "default_template", "Template", "templates");

if ($method == "POST")
{

	if ($form->save())
	{
		redirect("sites.php");	
	}
}

if ($site_id)
{
	$site->load($site_id);
	$title = "Edit Site Details for {$site->site_name}";
}
else
{
	$title = "Add a New Site";
}

$script .= $form->writeScript();

require_once admin_begin_page;

$form->drawForm();

require_once admin_end_page;
?>