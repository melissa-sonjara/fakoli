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
Fakoli::usingFeature("tab_bar", "data_view");

require_once "include/image_gallery_tabs.inc";

Fakoli::assertRole("admin");

$gallery_id = checkNumeric($_GET["gallery_id"]);

if (!$gallery_id) redirect("/admin/image_galleries");

$tabs = imageGalleryTabs($gallery_id);

$gallery = new ImageGallery($gallery_id);

$title = "Images in {$gallery->gallery_name}";

$view = new ImageGalleryListView($gallery);
	
$view->writeScript();

$tabs->writeHTML();
?>
	<div id="image_gallery_detail"><div id="image_detail">
<?php
$view->drawView();
?>
<br/><br/>
<a class="button" href="/admin/image_form?gallery_id=<?echo $gallery_id?>">Add a New Image</a>
</div></div>
<?
