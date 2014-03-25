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

$gallery_id = checkNumeric($_GET["gallery_id"]);

if (!$gallery_id)
{
	redirect("/index");
	return;
}

$gallery = new ImageGallery($gallery_id);

if (!$gallery->published)
{
	redirect("/index");
	return;
}

$images = $gallery->Images("ORDER BY sort_order");

$page->page_title = $gallery->gallery_name;

if ($gallery->image_link_mode == "Thumbnail")
{
	$linkFormat = "go(\"{related_resource}\"); return false";
}
else
{
	$linkFormat = "lightbox({image_id}); return false";
}

$script .= <<<ENDSCRIPT
<script type="text/javascript">
function lightbox(id)
{
	modalPopup('Image Gallery', '/action/image/simple_lightbox_view?image_id=' + id, 'auto', 'auto'); 
}
</script>
ENDSCRIPT;

$thumbnail_size = $gallery->thumbnail_size;
if (!$thumbnail_size) $thumbnail_size = 125;

echo $gallery->description;

$view = new ImageGalleryView($images, $linkFormat, "images", $thumbnail_size);
$view->spacing = "6px";
$view->styles = "margin-left: 40px";
$view->drawGallery();
?>