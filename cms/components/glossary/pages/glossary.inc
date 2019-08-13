<?php 
Fakoli::using("glossary","text_lookup");
Fakoli::usingFeature("data_view", "filter_form");

$menu_item = "Glossary";

$glossary = Query::create(Glossary, "ORDER BY term ASC")->execute();
$termLists = new GlossaryView($glossary);

echo TextLookup::getText("glossary_intro");

$termLists->draw();
?>