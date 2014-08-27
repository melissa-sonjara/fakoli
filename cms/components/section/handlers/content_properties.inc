<?php
Fakoli::using("section", "field_renderers", "role");
Fakoli::usingFeature("auto_form");

Fakoli::assertRole("admin");

$scope = $_GET["scope"];
$ids = checkNumeric($_GET["section_content_id"]);

$qs = "section_content_id[]=".implode("&section_content_id[]=", $_GET["section_content_id"]);

$sectionContent = new SectionContent();
$filter = new InclusionFilter("section_content_id");

if ($scope == "template" || $scope == "all")
{
	$filter->add("template");

}

if ($scope == "role" || $scope == "all")
{
	$filter->add("role");
}

if ($scope == "permissions" || $scope == "all")
{
	$filter->add("permissions");
}

if ($scope == "ssl" || $scope == "all")
{
	$filter->add("use_SSL");
}

if ($scope == "override_page_title" || $scope == "all")
{
	$filter->add("override_page_title");
}

$sectionContent->filter = $filter;

// Default dialog to properties for first selected item
$sectionContent->load($ids[0]); 

$form = new AutoForm($sectionContent, "POST", "/action/section/content_properties?$qs&scope=$scope", "SectionContent_form");
$form->ajaxSubmit("function(result) {sectionContentManager.propertiesChanged(result);}", "function() {document.id('{$form->id}_error').set('text','Failed to communicate with server'); }");
$form->button("Cancel", "sectionContentManager.cancel()", null, true);


if ($scope == "template" || $scope == "all")
{
	$templateSelect = new TemplateSelectFieldRenderer($form, "template", "Template", "templates");
}

if ($scope == "role" || $scope == "all")
{
	$roleList = new CheckListFieldRenderer($form, "role", "Role", SiteRole::getRolesArray());
}

if ($scope == "permissions" || $scope == "all")
{
	$permissionsList = Settings::createPermissionsFieldRenderer($form, "permissions", "Permissions");
	if ($permissionsList) $permissionsList->colspan = 2;
}

$form->submitLabel = "Update Properties";

if ($method == "POST")
{
	$sectionContent->fromPOST();
	
	if ($scope == "role" || $scope == "all")
	{
		$sectionContent->role = isset($_POST["role"]) ? implode(",", $_POST["role"]) : "";
	}
	
	if ($scope == "permissions" || $scope == "all")
	{
		$sectionContent->permissions = isset($_POST["permissions"]) ? implode(",", $_POST["permissions"]) : "";
	}

	$form->postProcessFields($sectionContent->primary_key, $filter);
	
	foreach($ids as $id)
	{
		$sectionContent->section_content_id = $id;
		$sectionContent->save();
	}
	
	Fakoli::end("OK");
	return;
}

echo $form->writeScript();
$form->drawForm();
?>