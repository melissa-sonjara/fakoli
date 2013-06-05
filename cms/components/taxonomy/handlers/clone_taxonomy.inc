<?php
Fakoli::using("taxonomy");

Fakoli::assertRole("admin");

$taxonomy_id = checkNumeric($_GET["taxonomy_id"]);

if (!$taxonomy_id)
{
	throw new FakoliException("No taxonomy provided");
}

$taxonomy = new Taxonomy($taxonomy_id);

$clone = new Taxonomy();

$clone->filter = new InclusionFilter("taxonomy_id", "taxonomy_name", "identifier");

$form = new AutoForm($clone, "POST", "/action/taxonomy/clone_taxonomy?taxonomy_id=$taxonomy_id", "CloneTaxonomy_form");
$form->required("taxonomy_name", "identifier");

$form->ajaxSubmit("function(result) {TaxonomyAdmin.cloneTaxonomyResponse(result);}", "function() {\$('{$form->id}_error').set('text','Failed to communicate with server'); }");
$form->button("Cancel", "TaxonomyAdmin.closeCloneDialog()", null, true);
$form->customSaveHandler = array(TaxonomyManager, cloneTaxonomy);

if ($method == "POST")
{
	if ($form->save())
	{
		Fakoli::end($clone->taxonomy_id);
	}
}

if (!$clone->taxonomy_name) 
{
	$clone->taxonomy_name = "Copy of {$taxonomy->taxonomy_name}";
	$clone->identifier = "copy_of_{$taxonomy->identifier}";
}

$script .= $form->writeScript();
$form->drawForm();