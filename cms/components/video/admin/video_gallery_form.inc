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
Fakoli::usingFeature("auto_form");

require_once "include/video_gallery_tabs.inc";

$menu_item = "Videos";

$video_gallery_id = checkNumeric($_GET["video_gallery_id"]);
$allowDelete = false;

if ($video_gallery_id)
{
	$gallery = new VideoGallery($video_gallery_id);
	$allowDelete = ($gallery->countVideos() == 0); 
}
else
{
	$gallery = new VideoGallery();
	$gallery->owner_id = $user->user_id;
}

$gallery->filter = new ExclusionFilter("read_access", "write_access");

$tabs = videoGalleryTabs($video_gallery_id);

$form = new AutoForm($gallery);
$form->allowDelete = $allowDelete;
$form->hide("owner_id");

if ($method == "POST")
{
	if ($form->save())
	{
		$d = $form->data->getGalleryDirectory();
		if (!is_dir($d) && !file_exists($d))
		{
			mkdir($d);
		}
		
		if ($form->deleted) redirect("/admin/video_galleries");
		
		$tabs->queryString = "video_gallery_id=".$form->data->video_gallery_id;
		$tabs->next();
	}
}


$script .= $form->writeScript();

$tabs->writeHTML();
?>
<div id="tab_border" style="margin-right: -2px">
<?
$form->drawForm();
?>
</div>