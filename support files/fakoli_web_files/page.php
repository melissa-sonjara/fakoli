<?php
require_once "include/config.inc";
require_once "cms/core.inc";

Fakoli::using("page", "component", "section", "settings");

$page_id = checkNumeric($_GET["page_id"]);
$identifier = checkIdentifier($_GET["identifier"]);
$sectionIdentifier = checkIdentifier($_GET["section"]);

Fakoli::sendPage($sectionIdentifier, $identifier, $page_id);

?> 