<?php

Fakoli::using("module");
Fakoli::usingFile("framework/data_view.inc");

Fakoli::assertRole("admin");

$menu_item = "Modules";

$title = "Manage Modules";

$modules = query(Module, "ORDER BY title");

$count = 0;

function getModuleType($module)
{
	return prettify($module->content_type);
}

$table = new DataListView($modules, "modules");
$table->column("Title", "<a href='{getAdminForm()}?module_id={module_id}'>{title}</a>", true, 'width: 40%')
	  ->column("Type", getModuleType, true, 'width: 10%')
	  ->column("Code File", "{php_code_file}", true, "width: 50%");
$table->sortable = true;
$table->filter = true;
$table->pageSize = 20;
$table->emptyMessage = "No Modules have been added.";

$script = $table->writeScript();



$table->drawView();

?>
<br/>
<br/>
<a class='button' href="/admin/code_module_form">Add a Code Module Form</a>&nbsp;&nbsp;
<a class='button' href="/admin/html_module_form">Add an HTML Module</a>&nbsp;&nbsp;
<a class='button' href="/admin/menu_module_form">Add a Menu Module</a>
