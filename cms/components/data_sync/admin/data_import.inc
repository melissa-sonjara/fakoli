<?php
Fakoli::using("data_sync");
Fakoli::usingFeature("auto_form");

$menu_item = "Data Import";

$dummy = new DataImportFieldMapping();
$dummy->filter = new InclusionFilter("mapping_id");

$form = new AutoForm($dummy);

$form->submitLabel = "Upload Data";
$form->customSaveHandler = function ($form)
{
	return true;
};

$adapters = DataSyncManager::getAvailableDataAdapters();
$adapters = array_combine($adapters, $adapters);
$form->dropdown("file_format", "Data File Format", $adapters);

$upload = new FileUploadFieldRenderer($form, "data_file", "Data File", function($field)
{
	$format = $_POST["file_format"];
	$adapter = DataSyncManager::getDataAdapter($format);
	$file = $adapter->uploadDataImportFile($field);
	$_SESSION["data_import_file"] = $file;
	$_SESSION["data_file_format"] = $format;
	
	trace(print_r($_SESSION, true), 3);
});

$tabs = DataSyncManager::getAdminImportTabs();

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

?>