<?php
require_once "../include/config.inc";
require_once "../cms/core.inc";

Fakoli::using("component");

Fakoli::assertRole("admin");

$identifier = $_GET["identifier"];

if (!$identifier) $identifier = "index";

$view = new AdminPageView($identifier);
$view->drawView();

?>