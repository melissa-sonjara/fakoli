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
Fakoli::usingFeature("auto_form");

require_once "include/image_gallery_tabs.inc";

$menu_item = "Images";

$gallery_id = checkNumeric($_GET["gallery_id"]);
$allowDelete = false;

if ($gallery_id)
{
	$gallery = new ImageGallery($gallery_id);
	$allowDelete = ($gallery->countImages() == 0); 
}
else
{
	$gallery = new ImageGallery();
	$gallery->owner_id = $user->user_id;
}

$gallery->filter = new ExclusionFilter("read_access", "write_access");

$tabs = imageGalleryTabs($gallery_id);

$form = new AutoForm($gallery);
$form->group("Gallery Properties", "gallery_name", "identifier", "description", "keywords", "hidden", "randomize_filenames");
$form->group("Display Options", "image_link_mode", "thumbnail_size", "thumbnail_format", "fix_orientation", "published");
$form->annotate("thumbnail_size", "Size of thumbnail images in the gallery display. Set as zero to use the site default thumbnail size");
$form->annotate("image_link_mode", "Specifies the behavior for image links in the gallery display");
$form->annotate("fix_orientation", "Automatically fix the orientation of JPEG photos uploaded from cameras, tablets and mobile phones");
$form->annotate("thumbnail_format", "Use PNG for high quality thumbnails (default) or JPEG for low bandwidth thumbnails.<br>JPEG files will have lower image quality and do not support transparency.");
$form->annotate("randomize_filenames", "Generate random unique names for uploaded files");

$linkModes = new SelectFieldRenderer($form, "image_link_mode", "Image Link Mode", ImageGallery::$imageLinkModes);
$thumbnailFormats = new SelectFieldRenderer($form, "thumbnail_format", "Thumbnail Format", ImageGallery::$thumbnailFormats);

$form->allowDelete = $allowDelete;
$form->hide("owner_id");
$form->unique("identifier");
$form->required("gallery_name", "identifier");

if ($method == "POST")
{
	if ($form->save())
	{
		$d = $form->data->getGalleryDirectory();
		if (!is_dir($d) && !file_exists($d))
		{
			mkdir($d);
		}
		
		if ($form->deleted) redirect("/admin/image_galleries");
		
		$tabs->queryString = "gallery_id=".$form->data->gallery_id;
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