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

$gallery_id = checkNumeric($_GET["gallery_id"]);

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

$tabs = ImageManager::imageGalleryTabs($gallery_id);
$styles .= ImageManager::getStyles();

$form = new AutoForm($gallery);
$form->group("Gallery Properties", "gallery_name", "identifier", "description", "hidden");
$form->group("Display Options", "image_link_mode", "thumbnail_size", "published");

$linkModes = ChecklistFieldRenderer($form, "image_link_mode", "Image Link Mode", ImageGallery::$imageLinkModes);

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
		
		if ($form->deleted) redirect("image_galleries");
		
		$tabs->queryString = "gallery_id=".$form->data->gallery_id;
		$tabs->next();
	}
}


$script .= $form->writeScript();

$tabs->writeHTML();
?>
<div id="tab_border">
<?
$form->drawForm();
?>
</div>