<?php

Fakoli::using("component");

Fakoli::assertRole("admin");

require_once "include/component_tabs.inc";

$component_id = checkNumeric($_GET["component_id"]);

$tabs = componentListTabs();

$menu_item = "Components";
$title = "Components Pages";

if ($component_id)
{
	$component = new Component($component_id);
	$view = new ComponentPageListView($component);
	$title .= " for {$component->name}";
	$tabs = componentFormTabs($component_id);
}
else
{
	$view = new ComponentPageListView();
}

$script .= $view->writeScript();


$tabs->writeHTML();
?>
<div id="form"  style="clear:left;border:solid 1px #000; padding: 4px;">
<?
$view->drawView();
?>
<br/>
<a class="button" href="/action/component/scan"> Scan for Component Updates </a>
<br/>
<br/>
</div>
