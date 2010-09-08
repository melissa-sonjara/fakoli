<?php
Fakoli::using("image");

$image_id = checkNumeric($_GET["image_id"]);

$mgr = new ImageManager();
$mgr->renderImage($image_id);

?>