<?php

Fakoli::using("component", "field_renderers", "role");
Fakoli::usingFile("framework/auto_form.inc");

require_once "include/component_tabs.inc";

$component_page_id = checkNumeric($_GET["component_page_id"]);
$page = new ComponentPage($component_page_id);

if (!$component_page_id)
{
	redirect("/");
}

$menu_item = "Components";

$tabs = componentPageTabs($component_page_id);

$title = "Component Page: {$page->identifier}";

$form = new AutoForm($page);
$form->readonly("component", "identifier", "server_path");
$form->hide("scanned");
$form->button("Back to Component Pages", "/admin/component_pages");
$templateSelect = new TemplateSelectFieldRenderer($form, "template", "Template", "templates");


$roleList = new CheckListFieldRenderer($form, "role", "Role", SiteRole::getRolesArray());

if ($method == "POST")
{
	if ($form->save())
	{
		redirect("/admin/component_pages");
	}
}

$script .= $form->writeScript();

$tabs->writeHTML();
?>
<div id="form"  style="clear:left;border:solid 1px #000; padding: 4px;">
<?
$form->drawForm();
?>
<br/>
</div>