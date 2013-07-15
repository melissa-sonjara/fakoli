<?php
Fakoli::using("data_sync");
Fakoli::usingFeature("spreadsheet_form");

$class = $_SESSION["data_import_class"];

$menu_item = "Data Import";
$page_title = "Choose Fields to Import";

$columns = DataSyncManager::getImportColumns($class);
$fieldMappings = DataSyncManager::getFieldMappings($class);

$options = array_combine($columns, $columns);

$mapping = new DataImportFieldMapping();
$form = new AutoForm($mapping);
$form->hide("class");
$columnSelect = new ChecklistFieldRenderer($form, "import_column", "Import Column", $options);
$columnSelect->setSize(250, 200);
$columnSelect->dropdown = true;
$columnSelect->dropdownMessage = "Click to Select Columns";

$form->readonly("client_field");
$form->getRenderer("client_field")->template = "prettify";

$spreadsheet = new SpreadsheetForm($form, $fieldMappings, 0);

$spreadsheet->submitLabel = "Save Field Mapping";
$spreadsheet->button("Clear Field Mapping", "/action/data_sync/clear_mapping");

$tabs = DataSyncManager::getAdminImportTabs();

if ($method == "POST")
{
	if ($spreadsheet->save())
	{
		$tabs->next();
	}
}

$script .= $spreadsheet->writeScript();

$tabs->writeHTML();
?>
<div style="clear:left; border: solid 1px #000; width: 100%; margin: 0; padding: 4px;">
<?
$spreadsheet->drawForm();
?>
</div>