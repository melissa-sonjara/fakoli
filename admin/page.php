<?php
require_once "../include/config.inc";
require_once "cms/core.inc";

Fakoli::using("component");

$identifier = $_GET["identifier"];

if (!$identifier) $identifier = "index";

if ($identifier != "login") Fakoli::assertRole("admin", "/admin/login");

$isAdmin = true;

$view = new AdminPageView($identifier);
$view->drawView();

?>