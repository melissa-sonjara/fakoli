<?php
Fakoli::using("data_sync");
Fakoli::usingFeature("spreadsheet_form");

$menu_item = "Data Import";

DataSyncManager::getSyncTargets();

$class = $_SESSION["data_import_class"];
$format = $_SESSION["data_file_format"];
$file = $_SESSION["data_import_file"];

$import_profile_id = $_SESSION["import_profile_id"];

$multiProfile = Settings::getValue("data_sync", "multiple_import_profiles");

if ($multiProfile)
{
	if (!$import_profile_id)
	{
		redirect("data_import_field_mapping");
	}
}
else
{
	$import_profile_id = 0;
}

$obj = new $class;

$name = $obj->prettifyClassName(true);

$fieldMapping = DataSyncManager::getFieldMapping($class, $import_profile_id);

if (!DataSyncManager::fieldMappingHasMatchField($fieldMapping))
{
	redirect("data_import_field_mapping?import_profile_id={$import_profile_id}");
}

$filter = DataSyncManager::generateFilter($fieldMapping);
$filter->add($obj->primary_key);

$obj->filter = $filter;

$page_title = "Select $name to Import";

$items = DataSyncManager::createImportItems($class, $import_profile_id, $file, $format);

trace("Mapping ".count($items)." items", 3);
$form = new AutoForm($obj);

if (is_array($obj->spreadsheetFieldSizes))
{
	foreach($obj->spreadsheetFieldSizes as $field => $size)
	{
		$form->getRenderer($field)->size = $size;	
	}
}

DataSyncManager::extendForm($form);


$isChunked = Settings::getValue("data_sync", "import_in_chunks") ? "true" : "false";
$pageSize = Settings::getValue("data_sync", "records_per_page");
$totalRecords = DataSyncManager::$totalRecords;

$offset = checkNumeric($_GET["offset"]);
if ($offset == '') $offset = 0;
if ($pageSize == '') $pageSize = 0;
if ($totalRecords == '') $totalRecords = 0;

$spreadsheet = new SpreadsheetForm($form, $items, 0, "POST", "", "", array(DataSyncManager, onItemRow), array(DataSyncManager, isRowSelected));
$spreadsheet->formCSS = "list spreadsheet small";

$spreadsheet->submitLabel = "Import $name";
$spreadsheet->emptyMessage = "<strong>No valid records were found in the import file</strong>";

$tabs = DataSyncManager::getAdminImportTabs();

if ($method == "POST")
{
	if ($spreadsheet->save())
	{
		Fakoli::notify("Records imported");

		$spreadsheet = new SpreadsheetForm($form, $items, 0, "POST", "", "", array(DataSyncManager, onItemRow), array(DataSyncManager, isRowSelected));
		$spreadsheet->formCSS = "list spreadsheet small";
		
		$spreadsheet->submitLabel = "Import $name";
		$spreadsheet->emptyMessage = "<strong>No valid records were found in the import file</strong>";
		
	}
}

$script .= $form->writeScript();
$script .= <<<ENDSCRIPT
<script type='text/javascript'>

var syncManager;
		
window.addEvent('domready', function()
{
	syncManager = new DataSyncManager('{$spreadsheet->id}', 'sync_controls', {chunked: $isChunked, offset: $offset, pageSize: $pageSize, totalRecords: $totalRecords});
});
</script>
ENDSCRIPT;

$tabs->writeHTML();
?>
<div class="tab_border">
<?
if (count($spreadsheet->forms))
{
?>
<div id="sync_controls"></div>
<?
}
?>
<div style='margin-top: 10px; width: 100%; overflow-y: auto'>
<?
$spreadsheet->drawForm();
?>
</div></div>