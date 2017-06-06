<?php
Fakoli::using("developer_tools");
Fakoli::usingFeature("grouped_data_view");

$page_title = "Event Map";
$menu_item = "Event Map";


$eventMap = EventMapRecord::convertMap(ComponentManager::getEventHandlers());


$table = new GroupedDataListView($eventMap, "event_map");
$table->column("Component", "{component}", true, "width: 20%")
	  ->column("Priority", "{priority}", true, "width: 10%; text-align: right")
	  ->column("Handler", "{handler}", true, "width: 60%")
	  ->column("Exists?", "<img src='/fakoli/images/{exists:on/off}.png' alt='{exists:Yes/No}'/>", true, "text-align: center");

foreach(array_keys($eventMap) as $event)
{
	$table->group($event, $event);
}

$table->mode = "tree";
$table->groupAsWorksheets = true;
$table->excelFile = codify($config['site_name']."_event_map").".xls";

$script .= $table->writeScript();
$table->drawView();