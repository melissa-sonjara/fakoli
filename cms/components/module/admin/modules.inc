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

require_once admin_begin_page;

$table->drawView();

?>
<br/>
<br/>
<input type="button" class="button" value="Add a Code Module" onclick="go('code_module_form');"/>&nbsp;&nbsp;
<input type="button" class="button" value="Add a Query Module" onclick="go('query_module_form');"/>&nbsp;&nbsp;
<input type="button" class="button" value="Add an HTML Module" onclick="go('html_module_form');"/>&nbsp;&nbsp;
<input type="button" class="button" value="Add a Menu Module" onclick="go('menu_module_form');"/>
<?
require_once admin_end_page;
?>