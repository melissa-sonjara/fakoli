<?php

Fakoli::using("admin");
Fakoli::assertRole("admin");

AdminManager::startFrame("banner");
?>
<form id="search" target='Two' method='POST' action='/action/online_help/search'>
<input type='text' id='searchterms' name='search_terms'/>&nbsp;<input type='submit' name='submit' value="Search" class="button"/></form>
<h2>Fakoli Online Help</h2>
<?
AdminManager::endFrame();
?>