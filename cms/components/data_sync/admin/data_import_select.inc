<?php
Fakoli::using("data_sync");
Fakoli::usingFeature("spreadsheet_form");

$menu_item = "Data Import";

DataSyncManager::getSyncTargets();

$class = $_SESSION["data_import_class"];

$obj = new $class;

$name = $obj->prettifyClassName(true);

$mappedFields = Query::create(DataImportFieldMapping, "WHERE class=:cl AND import_column != ''")
					 ->bind(":cl", $class)
					 ->execute();

$filter = new InclusionFilter();
$filter->add($obj->primary_key);
foreach($mappedFields as $field)
{
	$filter->add($field->client_field);
}


$obj->filter = $filter;

$page_title = "Select $name to Import";

$items = DataSyncManager::createImportItems($class);

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
$totalRecords = DataSyncManager::countRecords();
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
		$tabs->next();
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
<div style="clear:left; border: solid 1px #000; width: 100%; margin: 0; padding: 4px;">
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