<?php

$gallery_id = checkNumeric($_GET["gallery_id"]);

if (!$gallery_id) return "";

$gallery = new ImageGallery($gallery_id);
$list = new ImageListView($gallery->Images(), "javascript:imagePicker.selectImage({image_id}, '{title}')");
$list->drawGrid();
?>