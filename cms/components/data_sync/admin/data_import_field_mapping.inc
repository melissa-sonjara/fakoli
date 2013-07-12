<?php
Fakoli::using("data_sync");
Fakoli::usingFeature("spreadsheet_form");

$class = $_SESSION["data_import_class"];

$menu_item = "Data Import";
$page_title = "Choose Fields to Import";

$fieldMappings = DataSyncManager::getFieldMappings($class);
$columns = DataSyncManager::getImportColumns();

$options = array_combine($columns, $columns);

$mapping = new DataImportFieldMapping();
$form = new AutoForm($mapping);
$form->hide("class");
$columnSelect = new ChecklistFieldRenderer($form, "import_column", "Import Column", $options);
$columnSelect->setSize(150, 150);
$columnSelect->dropdown = true;
$columnSelect->dropdownMessage = "Click to Select Columns";

$form->readonly("client_field");
$form->getRenderer("client_field")->template = "prettify";

$spreadsheet = new SpreadsheetForm($form, $fieldMappings, 0);

$spreadsheet->submitLabel = "Save Field Mapping";

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