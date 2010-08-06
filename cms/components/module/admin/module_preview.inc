<?php
require_once "../../include/config.inc";
require_once "../../include/permissions.inc";
require_once "../../datamodel/program.inc";

require_once "../components/views/module_view.inc";

//$menu_item = "Publications";

$module_id = checkNumeric($_GET["module_id"]);

$module = new Module($module_id);

$view = ModuleView::create($module);

$title = $module->title;

require_once admin_begin_page;

?>
<div style="clear:left; border: solid 1px #000; width: 100%; margin: 0; padding: 4px;">
<?
echo $view->drawView();
?>
</div><br/>
<?
require_once admin_end_page;
?>
