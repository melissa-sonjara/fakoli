<?php
require_once "../../../include/config.inc";
require_once "../../../include/permissions.inc";

$bytes = getRemote($_GET["img"]);
$img = imageCreateFromString($bytes);
$width = imagesx($img);
$height = imagesy($img);
imageDestroy($img);

echo "$width,$height";
?>