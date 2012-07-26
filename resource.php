<?php
$isResource = true;

require_once "include/config.inc";
require_once "cms/core.inc";

$path = $_GET["path"];
$component = $_GET["component"];

Fakoli::sendResource($path, $component);

?>