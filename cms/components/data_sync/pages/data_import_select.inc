<?php
Fakoli::using("data_sync");
Fakoli::usingFeature("spreadsheet_form");

DataSyncManager::getSyncTargets();

$class = $_SESSION["data_import_class"];

$obj = new $class;

$name = $obj->prettifyClassName(true);

$mappedFields = Query::create(DataImportFieldMapping, "WHERE class=:cl AND import_column != ''")
					 ->bind(":cl", $class)
					 ->execute();

$filter = new InclusionFilter();
foreach($mappedFields as $field)
{
	$filter->add($field->client_field);
}


$obj->filter = $filter;

$page->page_title = "Select $name to Import";

$items = DataSyncManager::createImportItems($class);

$form = new AutoForm($obj);

if (is_array($obj->spreadsheetFieldSizes))
{
	foreach($obj->spreadsheetFieldSizes as $field => $size)
	{
		$form->getRenderer($field)->size = $size;	
	}
}

DataSyncManager::extendForm($form);

/*
$form->getRenderer("telephone")->size = 15;
$form->getRenderer("alt_phone")->size = 15;
$form->getRenderer("email_address")->size = 25;
$form->getRenderer("contact_name")->size = 25;
$form->getRenderer("client")->size = 30;


$externalRefAlias = Settings::getValue("griot_settings", "external_reference_alias");
$form->alias("external_reference", $externalRefAlias);
*/

$spreadsheet = new SpreadsheetForm($form, $items, 0, "POST", "", "", array(DataSyncManager, onItemRow), array(DataSyncManager, isRowSelected));
$spreadsheet->formCSS = "list spreadsheet small";

$spreadsheet->submitLabel = "Import $name";

$tabs = DataSyncManager::getImportTabs();

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
	syncManager = new DataSyncManager('{$spreadsheet->id}', 'sync_controls');
});
</script>
ENDSCRIPT;

$tabs->writeHTML();
?>
<div style="clear:left; border: solid 1px #000; width: 100%; margin: 0; padding: 4px;">
<div id="sync_controls"></div>
<div style='margin-top: 10px; width: 100%; overflow-y: auto'>
<?
$spreadsheet->drawForm();
?>
</div></div>