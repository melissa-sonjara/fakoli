<?php
Fakoli::using("data_sync");

$targets = DataSyncManager::getSyncTargets();

$options = array();

foreach($targets as $target)
{
	$obj = new $target;
	
	$options[$target] = $obj->prettifyClassName(true);
}


$dummy = new DataImportFieldMapping();
$dummy->filter = new InclusionFilter("mapping_id");

$page->page_title = "Select Data Type for Import";

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
<div style="clear:left; border: solid 1px #000; width: 100%; margin: 0; padding: 4px;">
<?
$form->drawForm();
?>
</div>
<?

function selectImportTarget($form)
{
	$_SESSION["data_import_class"] = $_POST["target"];
	return true;
}
