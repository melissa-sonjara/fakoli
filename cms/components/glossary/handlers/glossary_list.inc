<?php 
Fakoli::using("glossary");

$glossary = Query::create(Glossary, "WHERE display=:d ORDER BY term ASC")->bind(":d",1)->execute();
$terms = new GlossaryView($glossary);
$terms->glossaryJsonFormat();
?>