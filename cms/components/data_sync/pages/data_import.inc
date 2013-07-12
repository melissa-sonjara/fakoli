<?php
Fakoli::using("data_sync");
Fakoli::usingFeature("auto_form");

$dummy = new DataImportFieldMapping();
$dummy->filter = new InclusionFilter("mapping_id");

$form = new AutoForm($dummy);

$form->submitLabel = "Upload Data";
$form->customSaveHandler = dataImportDummySave;

$upload = new FileUploadFieldRenderer($form, "csv_file", "CSV File", uploadDataImportFile);

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
<h3><?echo $library->name?></h3>
<?
$form->drawForm();
?>
</div>
<?
function dataImportDummySave($form)
{
	return true;
}

function uploadDataImportFile($field)
{
	global $config;
	
	$file = $_FILES[$field]["tmp_name"];
	
	$fullpath = $config["uploadbase"] . DIRECTORY_SEPARATOR . basename($file);
	
	if (file_exists($fullpath))
	{
		unlink($fullpath);
	}
	
	trace("Uploading $file to $fullpath", 3);
	
	move_uploaded_file($file, $fullpath);
	chmod($fullpath, 0755);

	$_SESSION["data_import_file"] = $fullpath;
}
?>