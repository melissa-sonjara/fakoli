<?php

Fakoli::using("taxonomy");
Fakoli::usingFeature("auto_form");

$menu_item = "Taxonomies";

$taxonomy_id = checkNumeric($_GET["taxonomy_id"]);

$taxonomy = new Taxonomy();

if ($taxonomy_id)
{
	$taxonomy->load($taxonomy_id);
}

$form = new AutoForm($taxonomy);
$form->required("identifier", "taxonomy_name");
$form->allowDelete = true;

$classes =  TaxonomyManager::getRegisteredClassOptions();

new CheckListFieldRenderer($form, "associated_classes", "Associated Content Items", $classes);

$tabs = TaxonomyManager::getTabs($taxonomy_id);

if ($method == "POST")
{
	if ($form->save())
	{
		$tabs->next();
	}
}

$script .= $form->writeScript();

$tabs->writeHTML();

?>
<div id="tab_border">
<?
echo $form->drawForm();
?>
</div>