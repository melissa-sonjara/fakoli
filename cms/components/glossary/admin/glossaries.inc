<?php
Fakoli::using("glossary");
Fakoli::usingFeature("data_view", "filter_form");

$menu_item = "Glossary";
$glossary = Query::create('Glossary', "ORDER BY term ASC")->execute();

$table = new DataListView($glossary, "Glossaries");
$table->column("Term", "<a href='/admin/glossary_form?glossary_id={glossary_id}'>{term}</a>")
->column("Description", "{definition}");

$table->sortable = true;
$table->filter = true;
$table->cssStyle = "width: 100%";
$table->excelFile = "glossary.xls";
$table->emptyMessage = "There are no terms in the glossary.";

$script .= $table->writeScript();

$table->drawView();
?>
<br/>
<a class='button' href='glossary_form'>Add a Term</a>
