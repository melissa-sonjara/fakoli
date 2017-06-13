<?php
Fakoli::using("developer_tools", "section");
Fakoli::usingFeature("grouped_data_view");

$page_title = "Permission Map";
$menu_item = "Permission Map";

$site_id = checkNumeric($_GET["site_id"]);

$sites = query(Site, "ORDER BY site_name");


if (!$site_id) $site_id = $sites[0]->site_id;

$tabs = new DataItemTabBar("tabs", $sites);

$permissions = Query::create(Settings, "WHERE options='Permissions' ORDER BY component")->execute();

$sectionContent = Query::create(SectionContent, "WHERE section_id IN (SELECT section_id FROM section WHERE site_id=:s) AND permissions IS NOT NULL AND permissions != ''")
					   ->bind(":s", $site_id)
				 	   ->execute();

$contentByPermission = array();

foreach($sectionContent as $content)
{
	$perms = explode(",", $content->permissions);
	foreach($perms as $perm)
	{
		if (!array_key_exists($perm, $contentByPermission))
		{
			$contentByPermission[$perm] = array();
		}
		
		$contentByPermission[$perm][] = $content;
	}
}

$groups = array();
foreach($permissions as $perm)
{
	$groups[$perm->format("{component}:{name}")] = $perm->format("{annotation} <span style='font-weight:normal;font-size:75%'>{component}:{name}</span>");
}


$table = new GroupedDataListView($contentByPermission, "content_by_permission");
$table->column("Identifier", "<strong>{identifier}</strong>", true, "width: 20%")
	  ->column("Roles", "{role:/,/, /}", true, "width: 30%")
	  ->column("Permissions", "{permissions:/,/, /}", true, "width:30%")
	  ->column("Template", "{template}", true);

foreach($groups as $key => $group)
{
	$table->group($group, $key);
}

$table->mode = "tree";
$table->groupAsWorksheets = false;
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