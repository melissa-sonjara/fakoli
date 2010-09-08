<?php

Fakoli::using("image");
Fakoli::usingFeature("tab_bar", "data_view");

require_once "include/image_gallery_tabs.inc";

Fakoli::assertRole("admin");

$gallery_id = checkNumeric($_GET["gallery_id"]);

if (!$gallery_id) redirect("/admin/image_galleries");

$tabs = imageGalleryTabs($gallery_id);

$gallery = new ImageGallery($gallery_id);
$images = $gallery->Images("ORDER BY title");

$title = "Images in {$gallery->gallery_name}";

$table = new DataListView($images, "images");
$table->column("Title", "<a href='image_form?image_id={image_id}'>{title}</a>", true, "width: 50%")
	  ->column("Image File", "{image_file}", true, "width: 50%");
$table->emptyMessage = "No images have been added to this gallery";
$table->pageSize = 20;
$table->sortable = true;
$table->filter = true;
	

$table->writeScript();

$tabs->writeHTML();
?>
	<div style="width:100%; border: solid 1px black; margin: 0; padding: 0">
	 <div style="padding: 8px">
<?
$table->drawView();
?>
<br/><br/>
<a class="button" href="/admin/image_form?gallery_id=<?echo $gallery_id?>">Add a New Image</a>
</div>
</div>
