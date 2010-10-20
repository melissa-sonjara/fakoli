<?php
/**************************************************************

 Copyright (c) 2010 Sonjara, Inc

 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:

 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.

 Except as contained in this notice, the name(s) of the above 
 copyright holders shall not be used in advertising or otherwise 
 to promote the sale, use or other dealings in this Software 
 without prior written authorization.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.

*****************************************************************/
 
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

$script .= <<<ENDSCRIPT
<script type="text/javascript">
window.addEvent('domready', function() { $('images_box').resizable(); });
</script>
ENDSCRIPT;

?>
	<div style="width: 100%; border: solid 1px #000;">
		<div style="overflow:auto; width:100%; height: 260px;border: none;border-bottom: solid 1px #000" id="images_box">
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
