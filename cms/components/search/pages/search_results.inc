<?php
Fakoli::using("search");

$text = $_GET["text"];

$mgr = new SearchManager();
$mgr->search($text)
	->sortByTitle()
	->showResults();
?>