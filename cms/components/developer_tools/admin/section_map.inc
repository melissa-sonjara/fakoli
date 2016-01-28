<?php
Fakoli::using("developer_tools", "section");
Fakoli::usingFeature("grouped_data_view");

$page_title = "Section Content Map";
$menu_item = "Section Map";

$site_id = checkNumeric($_GET["site_id"]);

$sites = query(Site, "ORDER BY site_name");


if (!$site_id) $site_id = $sites[0]->site_id;

$tabs = new DataItemTabBar("tabs", $sites);

$sections = Query::create(Section, "WHERE site_id=:s ORDER BY section")
				 ->bind(":s", $site_id)
				 ->execute();

$itemsByType = IndexedQuery::create(SectionContent, "WHERE section_id IN (SELECT section_id FROM section WHERE site_id=:s) ORDER BY identifier", "section_id")
						   ->bind(":s", $site_id)
						   ->execute();

$table = new GroupedDataListView($itemsByType, "items_by_type");
$table->column("Identifier", "<strong>{identifier}</strong>", true, "width: 20%")
	  ->column("Roles", "{role:/,/, /}", true, "width: 40%")
	  ->column("Permissions", "{permissions:/,/, /}", true)
	  ->column("Template", "{template}", true)
	  ->column("SSL", "{use_SSL:Yes/No}", true);

foreach($sections as $section)
{
	$table->group($section->section, $section->section_id);
}

$table->mode = "tree";
$table->groupAsWorksheets = true;
$table->filter = true;
$table->sortable = true;
$table->excelFile = codify($config['sitename']."_section_map").".xls";

$script .= $table->writeScript();

$tabs->writeHTML();
?>
<div class='tab_border'>
<?
$table->drawView();
?>
</div>