<?php

Fakoli::using("image");
Fakoli::usingFeature("tab_bar");

require_once "include/image_gallery_tabs.inc";

Fakoli::assertRole("admin");

$gallery_id = checkNumeric($_GET["gallery_id"]);

if (!$gallery_id) redirect("/admin/image_galleries");

$gallery = new ImageGallery($gallery_id);
$images = $gallery->Images("ORDER BY title");

$tabs = imageGalleryTabs($gallery_id);

$menu_item = "Images";

$count = 0;

$title = "Images in {$gallery->gallery_name}";

$grid = new ImageListView($images, "image_form?gallery_id={gallery_id}&image_id={image_id}");
$tabs->writeHTML();

?>
	<div style="width: 100%; border: solid 1px #000;">
		<div style="overflow:auto; width:100%; height: 260px;border: none">
<?
if (count($images) > 0)
{
	$grid->drawGrid();
?>

<br/>
<?
}
else
{
	?>
	<p style="margin: 8px"><br/><i>No images have been added to this gallery</i></p>
	<?
}
?>
</div>
<br/>
<div style="padding: 8px">
<a class="button" href="/admin/image_form?gallery_id=<?echo $gallery_id?>">Add a New Image</a>
</div>
</div>
