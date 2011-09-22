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
$video_id = checkNumeric($_GET["video_id"]);

$video = new Video();

if (!$video_gallery_id && !$video_id) redirect("/admin/video_galleries");

if ($video_id)
{
	$video->load($video_id);
	$title = "Edit Details for {$video->title}";	
	$video_gallery_id = $video->video_gallery_id;
}
else
{
	$video->user_id = $user->user_id;
	$video->video_gallery_id = $video_gallery_id;
}

$form = new AutoForm($video);
$form->hide("user_id", "video_gallery_id");
$form->required("title", "width", "height");
$form->annotate("width", "<em>Natural width of video in pixels</em>");
$form->annotate("height", "<em>Natural height of video in pixels</em>");
$form->annotate("video_file", "<em>Video File in FLV format</em>");
$form->annotate("image_file", "<em>First Frame in PNG or JPG format</em>");

$videoMgr = new VideoManager();
if ($videoMgr->hasFFMPEG())
{
	$form->hide("width", "height", "image_file");
}

$form->alias("image_id", "First Frame Image");

$uploadRenderer = new FileUploadFieldRenderer($form, "video_file", "Video File", uploadFLV);
$uploadRenderer->size = 60;

$imageUploadRenderer = new FileUploadFieldRenderer($form, "image_file", "First Frame Image", uploadImage);
$imageUploadRenderer->size = 60;

$form->allowDelete = true;

$redirect = "/admin/videos?video_gallery_id=$video_gallery_id";
$form->button("Cancel", $redirect);
$form->onSaveComplete = processVideo;

if ($method == "POST")
{
	if ($form->save())
	{
		redirect($redirect);
	}
}

$script .= $form->writeScript();


$tabs = videoGalleryTabs($video_gallery_id);
$tabs->page = "/admin/videos";

$tabs->writeHTML();
?>
<div id="tab_border" style="margin-right: -2px">
<?
$form->drawForm();
?>
</div>
<?
	
function uploadFLV($field, $video)
{
	global $config;
	
	trace("uploadFLV() called for $field", 3);
	
	if (!$_FILES[$field]) 
	{
		trace("No upload record for $field", 3);
		return false;
	}
	if ($_FILES[$field]["name"]=="") 
	{
		trace("Upload name is empty", 3);
		return false;
	}

	$folder = Settings::getValue("video", "video_folder");
	
	$dir = $config['homedir']. DIRECTORY_SEPARATOR . $folder;
	
	/* Copy across the uploaded file */

	trace("Upload Base: {$dir}", 3);
    
	$filename = $_FILES[$field]['name'];
	$ext = strtolower(substr($filename, -4));
	
	trace("Video format: $ext", 3);
	
	if ($ext != ".flv") 
	{
		throw new FakoliException("Unsupported video format");
	}
	
	$target = $dir . DIRECTORY_SEPARATOR . $filename;
	trace ("Uploading file to $target", 3);
	
	 if (file_exists($target))
	{
		// If a previous copy of the file already exists, remove it
		unlink($target);
	}
	
  	move_uploaded_file($_FILES[$field]["tmp_name"], $target);
  	chmod($target, 0755);
	$video->video_file = $filename;  	

	$videoMgr = new VideoManager();
	if ($videoMgr->hasFFMPEG())
	{
		$videoMgr->open($target);
		$video->width = $videoManager->getFrameWidth();
		$video->height = $videoManager->getFrameHeight();
	}
	
	return true;
}


function uploadImage($field, $video)
{
	global $config;
	
	trace("imageUploadHandler() called for $field", 3);
	
	if (!$_FILES[$field]) 
	{
		trace("No upload record for $field", 3);
		return false;
	}
	if ($_FILES[$field]["name"]=="") 
	{
		trace("Upload name is empty", 3);
		return false;
	}

	$folder = Settings::getValue("video", "video_folder");
	
	$dir = $config['homedir']. DIRECTORY_SEPARATOR . $folder;
	
	/* Copy across the uploaded file */

	trace("Upload Base: {$dir}", 3);
    
	$filename = $_FILES[$field]['name'];
	$ext = strtolower(substr($filename, -4));

	if ($ext != ".png" && $ext != ".jpg") 
	{
		throw new FakoliException("Unsupported image format");
	}
	
	$filename = tempnam($dir, "V") . $ext;
	
	$target = $dir . DIRECTORY_SEPARATOR . $filename;
	trace ("Uploading file to $target", 3);
	
	 if (file_exists($target))
	{
		// If a previous copy of the file already exists, remove it
		unlink($target);
	}
	
  	move_uploaded_file($_FILES[$field]["tmp_name"], $target);
  	chmod($target, 0755);
	$video->image_file = basename($filename);  	
	
  	return true;
}

function processVideo($form)
{
	
}
?>