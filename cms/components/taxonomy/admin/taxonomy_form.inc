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

if ($taxonomy_id)
{
	$form->button("Clone this Taxonomy", "TaxonomyAdmin.cloneTaxonomyDialog({$taxonomy_id}); return false;", null, true);
}

$classes =  TaxonomyManager::getRegisteredClassOptions();

new CheckListFieldRenderer($form, "associated_classes", "Associated Content Items", $classes);

$tabs = TaxonomyManager::getTabs($taxonomy_id);

if ($method == "POST")
{
	if ($form->save())
	{
		if ($form->deleted)
		{
			redirect("/admin/taxonomies");
		}
		else
		{
			$tabs->queryString = "taxonomy_id=".$form->data->taxonomy_id;
			$tabs->next();
		}
	}
}

$script .= <<<ENDSCRIPT
<script type='text/javascript' src='/components/taxonomy/js/taxonomy_admin.js'></script>
ENDSCRIPT;

$script .= $form->writeScript();

$tabs->writeHTML();

?>
<div id="tab_border">
<?
echo $form->drawForm();
?>
</div>