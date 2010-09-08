<?php

Fakoli::using("image");
Fakoli::usingFeature("data_view");

$menu_item = "Images";

$galleries = Query::create(ImageGallery, "ORDER BY gallery_name")->execute();

$table = new DataListView($galleries, "galleries");
$table->column("Image Gallery", "<a href='/admin/image_gallery_form?gallery_id={gallery_id}'>{gallery_name}</a>", true, "width: 55%")
	  ->column("Owner", "{getOwnerName()}", true, "width: 30%")
	  ->column("# Images", "{countImages()}", true, "width: 15%;text-align: right");
	  
$table->sortable = true;
$table->filter = true;
$table->pageSize = 10;
$table->emptyMessage = "No Image Galleries have been created.";
$script .= $table->writeScript();

$table->drawView();
?>
<br/>
<a href="/admin/image_gallery_form" class="button">Create an Image Gallery</a>