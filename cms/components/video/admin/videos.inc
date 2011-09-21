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

Fakoli::using("video");
Fakoli::usingFeature("data_view");

$menu_item = "Videos";

require_once "include/video_gallery_tabs.inc";

$video_gallery_id = checkNumeric($_GET["video_gallery_id"]);
$videos = Query::create(Video, "WHERE video_gallery_id=:g ORDER BY title")
			   ->bind(":g", $video_gallery_id)
			   ->execute();

$table = new DataListView($videos, "videos");

$table->column("Title", "<a href='/admin/video_form?video_gallery_id={video_gallery_id}&video_id={video_id}'>{title}</a>", true, "width: 35%")
	  ->column("Resolution", "{width} x {height}", true, "width: 10%")
	  ->column("Owner", "{User.first_name} {User.last_name}", true, "width: 25%")
	  ->column("Last Modified", "{last_modified}", true, "width: 20%");
	  
$table->sortable = true;
$table->filter = true;
$table->pageSize = 25;
$table->emptyMessage = "No videos have been uploaded to this gallery.";
$script .= $table->writeScript();

$tabs = videoGalleryTabs($video_gallery_id);

$tabs->writeHTML();
?>
<div id="tab_border" style="margin-right: -2px">
<?
$table->drawView();
?>
<br/>
<a href="/admin/video_form?video_gallery_id=<?echo $video_gallery_id?>" class='button'>Upload a Video</a>
<br/>
</div>