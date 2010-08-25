<?php

Fakoli::using("component");
Fakoli::usingFeature("auto_form");

require_once "include/component_tabs.inc";

$component_id = checkNumeric($_GET["component_id"]);
$component = new Component($component_id);

$title = "Component Definition";

$form = new AutoForm($component);
$form->readonly("name", "version", "author", "description", "component_path");
$form->hide("priority");
$form->button("Back to Components", "/admin/components");

if ($method == "POST")
{
	if ($form->save())
	{
		redirect("/admin/components");
	}
}

$tabs = componentFormTabs($component_id);

$script .= $form->writeScript();

$tabs->writeHTML();
?>
<div id="form"  style="clear:left;border:solid 1px #000; padding: 4px;">
<?
$form->drawForm();
?>
<br/>
</div>