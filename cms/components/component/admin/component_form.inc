<?php

Fakoli::using("component");
Fakoli::usingFile("framework/auto_form.inc");

$component_id = checkNumeric($_GET["component_id"]);
$component = new Component($component_id);

$title = "Component Definition";

$form = new AutoForm($component);
$form->readonly("name", "version", "author", "description", "component_path");
$form->button("Back to Components", "/admin/components");

if ($method == "POST")
{
	if ($form->save())
	{
		redirect("/admin/components");
	}
}

$script .= $form->writeScript();

$form->drawForm();
?>