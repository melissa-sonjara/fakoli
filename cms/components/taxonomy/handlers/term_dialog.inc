<?php

Fakoli::assertRole("admin");

$taxonomy_id = checkNumeric($_GET["taxonomy_id"]);
$term_id = checkNumeric($_GET["term_id"]);

if (!$taxonomy_id)
{
	Fakoli::end("No taxonomy specified");
}

$taxonomy = new Taxonomy($taxonomy_id);

$term = new TaxonomyTerm();
if ($term_id)
{
	$term->load($term_id);
}
else
{
	$term->taxonomy_id = $taxonomy_id;
}

$form = new AutoForm($term, "POST", "/action/taxonomy/term_dialog?taxonomy_id=$taxonomy_id&term_id=$term_id");
$form->hide("taxonomy_id");
$form->required("term");
$form->allowDelete = true;

$form->ajaxSubmit("function(result) {taxonomyAdmin.onSave(result);}", "function() {\$('{$form->id}_error').set('text','Failed to communicate with server'); }");
$form->button("Cancel", "taxonomyAdmin.closeTermDialog()", null, true);

if ($method == "POST")
{
	if ($form->save()) 
	{
		Fakoli::end("OK");
	}
	else
	{
		Fakoli::end($form->msg);
	}
}

echo $form->writeScript();
$form->drawForm();

