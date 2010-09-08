<?php

Fakoli::using("image");
Fakoli::usingFeature("auto_form");

require_once "include/image_gallery_tabs.inc";

$menu_item = "Images";

 function imageUploadHandler($field, $image)
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

	$gallery = $image->Gallery();
	
	$dir = $gallery->getGalleryDirectory();
	if (!file_exists($dir))
	{
		mkdir($dir);
	}
	
	/* Copy across the uploaded file */

	trace("Upload Base: {$dir}", 3);
    
	$filename = $_FILES[$field]['name'];
	$ext = strtolower(substr($filename, -4));
	
	trace("Image format: $ext", 3);
	
	if ($ext != ".jpg" && $ext != ".png") 
	{
		throw new FakoliException("Unsupported image format");
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
	$image->image_file = $filename;  	
  	return true;
}


$image_id = checkNumeric($_GET["image_id"]);
$gallery_id = checkNumeric($_GET["gallery_id"]);

if (!$image_id && !$gallery_id) redirect("/admin/image_galleries");

$image = new Image();

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
}

$tabs = imageGalleryTabs($gallery_id);

$form = new AutoForm($image);
$form->allowDelete = true;

$imageUpload = new FileUploadFieldRenderer($form, "image_file", "Image File", imageUploadHandler);
$imageUpload->size = 60;

$form->required( "title");
$form->hide("thumbnail", "gallery_id");

$imageTypeSelect = new SelectFieldRenderer($form, "image_type", "Image Type");
$imageTypeSelect->allowAddEntry();

$redirect = "/admin/images?gallery_id={$image->gallery_id}";
$form->button("Cancel", $redirect);

if ($method == "POST")
{
	if ($form->save())
	{
		redirect($redirect);
	}
}


$script = $form->writeScript();

$tabs->page = "/admin/images";
$tabs->writeHTML();
?>
<div style="clear:left; border: solid 1px #000; width: 100%; margin: 0; padding: 4px;">
<h3>Image Details</h3>
<?
if ($image_id)
{
?>
<img src="<?echo $image->thumbnail ?> " alt="Small version of image"/><br/>

<?
}
$form->drawForm();
?>
</div>