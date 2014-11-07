<?php
Fakoli::using("data_sync");
Fakoli::usingFeature("auto_form");

$class = checkIdentifier($_GET["class"]);
$import_profile_id = checkNumeric($_GET["import_profile_id"]);

$profile = new DataImportProfile();

if ($import_profile_id)
{
	$profile->load($import_profile_id);
}
else
{
	$profile->class = $class;
}

$form = new AutoForm($profile, "POST", "/action/data_sync/import_profile_dialog?class=$class");
$form->ajaxSubmit("function(result) { importProfileManager.profileEdited(result);}", "function() {document.id('{$form->id}_error').set('text','Failed to communicate with server'); }");

$form->hide("class");
$form->required("profile");

if ($method == "POST")
{
	if ($form->save())
	{
		Fakoli::end($form->data->import_profile_id);
	}
	else
	{
		Fakoli::end($form->msg);
	}
}

$script .= $form->writeScript();
$form->drawForm();