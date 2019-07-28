<?php
Fakoli::using("section", "field_renderers", "role");
Fakoli::usingFeature("auto_form");

Fakoli::assertRole("admin");

if ($method != "POST")
{
	Fakoli::end("Protocol Error");
}

$save = checkNumeric($_GET["save"]);

$sectionContent = new SectionContent();
$sectionContent->fromJSON($_POST["data"]);

$form = new AutoForm($sectionContent, "POST", "/action/section/content_properties_json?save=1", "SectionContent_form");
$form->ajaxSubmit("function(result) {sectionContentManager.propertiesChanged(result);}", "function() {document.id('{$form->id}_error').set('text','Failed to communicate with server'); }");
$form->button("Cancel", "sectionContentManager.cancel()", null, true);

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

if ($save)
{
	if ($form->save())
	{
		Fakoli::JSONreturn($form->data);
	}
	else
	{
		Fakoli::JSONerror($form->msg);
	}
}

echo $form->writeScript();
$form->drawForm();
?>