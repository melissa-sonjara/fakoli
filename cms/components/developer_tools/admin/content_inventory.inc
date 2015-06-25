<?php
Fakoli::using("developer_tools");
Fakoli::usingFeature("grouped_data_view");

$page_title = "Content Inventory By Type";
$menu_item = "Content Inventory";

$itemsByType = array();

$itemsByType = ComponentManager::fireEvent("EnumerateItems", $itemsByType);

$table = new GroupedDataListView($itemsByType, "items_by_type");
$table->column("Identifier", "<strong>{identifier}</strong>", true, "width: 20%")
	  ->column("Title", "{DeveloperToolsManager::formatDefault}", true, "width: 40%")
	  ->column("Used in Section(s)", "{DeveloperToolsManager::formatSections}", true, "width: 40%");

foreach(array_keys($itemsByType) as $group)
{
	$table->group($group, $group);
}

$table->mode = "tree";
$table->groupAsWorksheets = true;
$table->excelFile = codify($config['sitename']."_content_inventory").".xls";

$script .= $table->writeScript();
$table->drawView();