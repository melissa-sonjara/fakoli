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
Fakoli::usingFeature("data_view");

$menu_item = "Images";

$galleries = Query::create(ImageGallery, "ORDER BY gallery_name")->execute();

function showPublished($gallery)
{
	$img = $gallery->published ? "on.png" : "off.png";
	$alt = $gallery->published ? "Enabled" : "Disabled";

	return "<img src='/fakoli/images/$img' alt='$alt' style='display: inline-block; vertical-align: middle; border: none; cursor: pointer'/>";
}

$table = new DataListView($galleries, "galleries");
$table->column("Image Gallery", "<a href='/admin/image_gallery_form?gallery_id={gallery_id}'>{gallery_name}</a>", true, "width: 55%")
	  ->column("Owner", "{getOwnerName()}", true, "width: 30%")
	  ->column("# Images", "{countImages()}", true, "width: 15%;text-align: right")
	  ->column("Published", showPublished, false, "text-align: center");
	  
$table->sortable = true;
$table->filter = true;
$table->pageSize = 10;
$table->emptyMessage = "No Image Galleries have been created.";
$script .= $table->writeScript();

$table->drawView();
?>
<br/>
<a href="/admin/image_gallery_form" class="button">Create an Image Gallery</a>