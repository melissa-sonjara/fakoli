<?php

Fakoli::using("component");

Fakoli::assertRole("admin");

require_once "include/component_tabs.inc";

$tabs = componentListTabs();

$menu_item = "Components";
$title = "Manage Components";


$components = query(Component, "ORDER BY name");

$view = new ComponentPageListView();

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
