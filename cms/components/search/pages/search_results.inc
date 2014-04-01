<?php
Fakoli::using("search");

$text = $_GET["text"];

$mgr = new SearchManager();
$mgr->useFacetFilter = true;

$mgr->search($text)
	->sortByRelevance()
	->showResults();
?>