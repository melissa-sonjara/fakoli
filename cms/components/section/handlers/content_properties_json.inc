<?php
Fakoli::using("section", "field_renderers", "role");
Fakoli::usingFeature("auto_form");

Fakoli::assertRole("admin");

$sectionContent = new SectionContent();
$sectionContent->fromJSON($_REQUEST["data"]);

$form = new AutoForm($sectionContent, "POST", "/action/section/content_properties_json?save=1", "SectionContent_form");
$form->ajaxSubmit("function(result) {sectionContentPropertyManager.propertiesChanged(result);}");
$form->button("Cancel", "sectionContentPropertyManager.cancel()", null, true);

$form->hide("section_id", "identifier");

$form->customSaveHandler = function($form)
{
	// No direct save to database from this form
	return true;
};

$templateSelect = new TemplateSelectFieldRenderer($form, "template", "Template", "none");
$roleList = new CheckListFieldRenderer($form, "role", "Role", SiteRole::getRolesArray());
$permissionsList = Settings::createPermissionsFieldRenderer($form, "permissions", "Permissions");
if ($permissionsList) $permissionsList->colspan = 2;

$form->submitLabel = "Update Properties";

if ($method == "POST")
{
	if ($form->save())
	{
		Fakoli::JSONreturn($form->data, false);
	}
	else
	{
		Fakoli::JSONerror($form->msg, false);
	}
}

echo $form->writeScript();
$form->drawForm();
?>