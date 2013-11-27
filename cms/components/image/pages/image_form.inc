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

$image_id = checkNumeric($_GET["image_id"]);
$gallery_id = checkNumeric($_GET["gallery_id"]);

if (!$image_id && !$gallery_id) redirect("image_galleries");

$image = new ImageRecord();

if ($image_id)
{
	$image->load($image_id);
	$gallery = $image->Gallery();
	$title = "Edit Details for {$image->title}";
}
else
{
	$gallery = new ImageGallery($gallery_id);
	$title = "Add a New Image to {$gallery->gallery_name}";
	$image->gallery_id = $gallery_id;
	$image->owner_id = $user->get($user->getPrimaryKey());
}

$tabs = ImageManager::imageGalleryTabs($gallery_id);
$styles .= ImageManager::getStyles();

$form = new AutoForm($image);
$form->allowDelete = true;

$imageUpload = new FileUploadFieldRenderer($form, "image_file", "Image File", array(ImageManager, imageUploadHandler));
$imageUpload->size = 60;

$form->required( "title");
$form->hide("thumbnail", "gallery_id", "owner_id");

$imageTypeSelect = new SelectFieldRenderer($form, "image_type", "Image Type");
$imageTypeSelect->allowAddEntry();

if($image->owner_id)
{
	$form->add(new StringFieldRenderer($form), "owner");
	$form->data->set("owner", $image->Owner()->format("{first_name} {last_name}"));
	$form->readOnly("owner");
}

$redirect = "images?gallery_id={$image->gallery_id}";
$form->button("Cancel", $redirect);

if ($method == "POST")
{
	if ($form->save())
	{
		redirect($redirect);
	}
}


$script = $form->writeScript();

$tabs->page = "images";
$tabs->writeHTML();
?>
<div id="tab_border">
<h3>Image Details</h3>
<?
if ($image_id)
{
?>
<a href="/action/image/show?image_id=<?echo $image_id?>" target="_blank"><img src="<?echo $image->getThumbnail(350) ?> " alt="Small version of image"/></a><br/>

<?
}
$form->drawForm();
?>
</div>