<?php

$book = $_GET["book"];
$page = $_GET["page"];


$helpBook = HelpBook::findByIdentifier($book);

$mgr = new OnlineHelpManager($helpBook);

$mgr->startFrame("banner");
if ($helpBook->searchable)
{
?>
<form id="search" target='Two' method='POST' action='/action/online_help/search'>
<input type='text' id='searchterms' name='search_terms'/>&nbsp;<input type='submit' name='submit' value="Search" class="button"/></form>
<?
}
?>
<h2><?echo $helpBook->title?></h2>
<?
$mgr->endFrame();
?>