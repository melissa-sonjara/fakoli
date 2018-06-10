<?php
Fakoli::using("data_sync");

$menu_item = "Data Import";

$targets = DataSyncManager::getSyncTargets();

$options = array();

foreach($targets as $target)
{
	$obj = new $target;
	
	$options[$target] = $obj->prettifyClassName(true);
}


$dummy = new DataImportFieldMapping();
$dummy->filter = new InclusionFilter("mapping_id");

$page_title = "Select Data Type for Import";

$form = new AutoForm($dummy);

$form->submitLabel = "Select Data Type";
$form->customSaveHandler = selectImportTarget;

$targetSelect = new RadioButtonFieldRenderer($form, "target", "Data to Import", $options);

$tabs = DataSyncManager::getImportTabs();

if ($method == "POST")
{
	if ($form->save())
	{
		$tabs->next();
	}
}

$script .= $form->writeScript();

$tabs->writeHTML();
?>
<div class="tab_border">
<?
$form->drawForm();
?>
</div>
<?

function selectImportTarget($form)
{
	$_SESSION["data_import_class"] = $_POST["target"];
	//AJG: Make sure that the import_profile_id is reset so that it isn't brought over from a previous import
	unset($_SESSION["import_profile_id"]);
	
	return true;
}
