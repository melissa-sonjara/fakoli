<?php
Fakoli::using("module");

$module_id = checkNumeric($_GET["module_id"]);

$module = new Module($module_id);

$view = ModuleView::create($module);

$title = $module->title;
?>
<div style="clear:left; border: solid 1px #000; width: 100%; margin: 0; padding: 4px;">
<?
echo $view->drawView();
?>
</div><br/>
<?

?>
