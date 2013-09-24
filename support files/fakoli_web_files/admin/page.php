<?php

$isAdmin = true;

require_once "../include/config.inc";
require_once "cms/core.inc";

Fakoli::using("component");

$identifier = checkIdentifier($_GET["identifier"]);

Fakoli::sendAdminPage($identifier);

?>