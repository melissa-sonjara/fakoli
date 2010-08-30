<?php

require_once "include/config.inc";
require_once "cms/core.inc";

$path = $_GET["path"];

Fakoli::sendResource($path);

?>