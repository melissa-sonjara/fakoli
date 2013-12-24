<?php

Fakoli::using("taxonomy");
Fakoli::usingFeature("data_view");

$taxonomies = Query::create(Taxonomy, "ORDER BY taxonomy_name")->execute();

$table = new DataListView($taxonomies, "taxonomies");

$table->column("Taxonomy", "<a href='taxonomy_form?taxonomy_id={taxonomy_id}'>{taxonomy_name}</a>", true)
	  ->column("Identifier", "<a href='taxonomy_form?taxonomy_id={taxonomy_id}'>{identifier}</a>", true)
	  ->column("Associated Classes", "{associated_classes:/,/, /}", true)
	  ->column("# of Terms", "{countTerms()}", true, "text-align: right");
	  
$table->pageSize = 20;
$table->sortable = true;
$table->filter = true;
$table->emptyMessage = "No taxonomies have been created yet.";

$script .= $table->writeScript();

$table->drawView();
?>
<br/>
<a class='button' href='taxonomy_form'>Add a New Taxonomy</a>