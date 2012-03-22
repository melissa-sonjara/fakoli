<?php

$isAdmin = true;

require_once "../include/config.inc";
require_once "cms/core.inc";

Fakoli::using("component");

$identifier = $_GET["identifier"];

if (!$identifier) $identifier = "index";

if ($identifier != "login") Fakoli::assertRole("admin", "/admin/login");

$view = new AdminPageView($identifier);
$view->drawView();

?>