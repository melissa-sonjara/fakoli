<?php

Fakoli::using("site");
Fakoli::using("field_renderers");
Fakoli::usingFile("framework/auto_form.inc");

Fakoli::assertRole("admin");

$menu_item = "Sites";

$site_id = checkNumeric($_GET["site_id"]);

$site = new Site();

$form = new AutoForm($site);

$form->required("title");
$form->allowDelete = true;
$redirect = "sites";
$form->button("Cancel", $redirect);
$templateSelect       = new TemplateSelectFieldRenderer($form, "default_template", "Default Template", "templates");
$printTemplateSelect  = new TemplateSelectFieldRenderer($form, "print_template", "Print Template", "templates");
$popupTemplateSelect  = new TemplateSelectFieldRenderer($form, "popup_template", "Popup Template", "templates");
$mobileTemplateSelect = new TemplateSelectFieldRenderer($form, "mobile_template", "Mobile Template", "templates");

if ($method == "POST")
{

	if ($form->save())
	{
		redirect("sites");	
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