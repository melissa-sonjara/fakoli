<?php
Fakoli::using("image");

$image_id = checkNumeric($_GET["image_id"]);
$size = checkNumeric($_GET["size"]);

$mgr = new ImageManager();
$mgr->renderThumbnail($image_id, $size);

?>