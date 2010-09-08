<?php

Fakoli::using("image");
Fakoli::usingFeature("data_view");

$menu_item = "Images";

$galleries = Query::create(ImageGallery, "ORDER BY gallery_name")->execute();

$table = new DataListView($galleries, "galleries");
$table->column("Image Gallery", "<a href='/admin/image_gallery_form?gallery_id={gallery_id}'>{gallery_name}</a>", true, "width: 60%")
	  ->column("Owner", "{getOwnerName()}");
	  
$table->sortable = true;
$table->filter = true;
$table->pageSize = 10;
$table->emptyMessage = "No Image Galleries have been created.";
$script .= $table->writeScript();

$table->drawView();
?>
<br/>
<a href="/admin/image_gallery_form" class="button">Create an Image Gallery</a>