<?php


Fakoli::using("component");
Fakoli::usingFile("framework/data_view.inc");

Fakoli::assertRole("admin");

$menu_item = "Components";
$title = "Manage Components";


$components = query(Component, "ORDER BY name");

$table = new DataListView($components, "components");
$table->column("Component Name", "<a href='/admin/component_form?component_id={component_id}'>{name}</a><span style='font-size: 10px'>{description}</span>", true)
	  ->column("Version", "{version}", true)
	  ->column("Author",  "{author}", true);

$table->sortable = true;
$table->filter = true;
$table->pageSize = 10;

$script .= $table->writeScript();



$table->drawView();
?>
<br/>
<br/>
<a class="button" href="/action/component/scan"> Scan for Component Updates </a>
<?

?>