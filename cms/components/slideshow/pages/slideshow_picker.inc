<?php

Fakoli::using("image");
Fakoli::usingFeature("data_view");

$editor = $_GET["Editor"];

$galleries = Query::create('ImageGallery', "ORDER BY gallery_name")->execute();

$table = new DataListView($galleries, "galleries");

$table->column("Gallery", "<a href='#' onclick='picker.selectSlideshow(this, {gallery_id})'>{gallery_name}</a>", true, "width: 80%")
	  ->column("# of Photos", 'countPhotos', true, "width: 20%");
	  
$table->sortable = true;
$table->emptyMessage = "No image galleries have been created";
$table->pageSize = 5;
$table->zebra = false;

$script .= $table->writeScript();
$script .= <<<ENDSCRIPT
<script type="text/javascript" src="/fakoli/slideshow/slideshow_picker.js"></script>
<script type="text/javascript">
var picker;
window.addEvent('domready', function()
{
	picker = new SlideshowPicker('galleries', '{$editor}');
});
</script>
ENDSCRIPT;
$table->drawView();
?>
<br/>
<label for="width" style="display: inline-block; width: 70px;">Width: </label><input type="text" style="width: 100px" id="width" value="500"/><br/><br/>
<label for="height" style="display: inline-block; width: 70px;">Height: </label><input type="text" style="width: 100px" id="height" value="400"/><br/>
<br/>
<a href="#" onclick="picker.insertSlideshow()" class="button">Insert Slideshow</a>
<?

function countPhotos($gallery)
{
	return Query::create('ImageRecord', "WHERE gallery_id=:id AND include_in_slideshow=1")
				->bind(":id", $gallery->gallery_id)
				->executeValue("COUNT(1)");
}
?>
