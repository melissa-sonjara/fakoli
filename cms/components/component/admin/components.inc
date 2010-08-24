<?php


Fakoli::using("component");
Fakoli::usingFile("framework/data_view.inc");

Fakoli::assertRole("admin");

require_once "include/component_tabs.inc";

$tabs = componentListTabs();

$menu_item = "Components";
$title = "Manage Components";


$components = query(Component, "ORDER BY name");

$table = new DataListView($components, "components");
$table->column("Component Name", "<a href='/admin/component_form?component_id={component_id}'>{name}</a><span style='font-size: 10px'>{description}</span>", true)
	  ->column("Version", "{version}", true, "width: 10%; text-align: center")
	  ->column("Author",  "{author}", true, "width: 20%; text-align: center");

$table->sortable = true;
$table->filter = true;
$table->pageSize = 10;

$script .= $table->writeScript();


$tabs->writeHTML();
?>
<div id="form"  style="clear:left;border:solid 1px #000; padding: 4px;">
<?
$table->drawView();
?>
<br/>
<a class="button" href="/action/component/scan"> Scan for Component Updates </a>
<br/>
<br/>
</div>
