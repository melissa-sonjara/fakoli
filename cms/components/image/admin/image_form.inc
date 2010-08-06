<?php

Fakoli::using("image");
Fakoli::usingFile("framework/auto_form.inc");

Fakoli::assertRole("admin");

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


	/*$fileType = $_FILES[$field]["type"];

	$matchingType = "";
	foreach($types as $t)
	{
		if (is_integer(strpos($fileType, $t)))
    	{
			$matchingType = $t;
      		break;
		}
	}

	if ($matchingType == "") die("No matching MIME type found for ".$fileType);*/

  	/* Copy across the uploaded file */

	trace("Upload Base: {$config['homedir']}", 3);
    
	$filename = $_FILES[$field]['name'];
	$ext = strtolower(substr($filename, -4));
	$filename = substr($filename, 0, -4 );
	
	trace("Image format: $ext", 3);
	
	if ($ext != ".jpg" && $ext != ".png") die("Unsupported image format");
	
	$largeFileName = codify($filename).$ext;
	$smallFileName = codify($filename)."_thumb$ext";
	
	trace("Large Image File: $largeFileName", 3);
	trace("Small Image File: $smallFileName", 3);
	
	$dir = $config['homedir']."/uploaded_images";
	$target = "$dir/$largeFileName";
	trace ("Uploading file to $target", 3);
	
	if (!file_exists($dir))
	{
		// If the directory does not exist, create it 
		mkdir($dir);
	}
	else if (file_exists($target))
	{
		// If a previous copy of the file already exists, remove it
		unlink($target);
	}
	
  	move_uploaded_file($_FILES[$field]["tmp_name"], $target);
  	chmod($target, 0755);
 
 	$thumbSize = $config['thumbnail_size'];
 	
 	switch($ext)
 	{
 		case ".jpg":
 			$src = imagecreatefromjpeg($target);
 			break;
 			
 		case ".png":
 			$src = imagecreatefrompng($target);
 			break;
 	}

 	$fullWidth = imagesx($src);
	$fullHeight = imagesy($src);
	
	if ($fullWidth > $fullHeight)
	{
		$newWidth = $thumbSize;
		$newHeight = intval(($fullHeight * $thumbSize) / $fullWidth);
		
	}
	else
	{
		$newWidth = intval(($fullWidth * $thumbSize) / $fullHeight);
		$newHeight = $thumbSize;
	}

	$dst = imagecreatetruecolor($newWidth, $newHeight);
	imagecopyresampled($dst, $src, 0, 0, 0, 0, $newWidth, $newHeight, $fullWidth, $fullHeight);
	
	$target = "$dir/$smallFileName";
	if (file_exists($target))
	{
		// If a previous copy of the file already exists, remove it
		unlink($target);
	}
	
	switch($ext)
	{
	case ".jpg":
		
		imagejpeg($dst, $target, 85);
		break;
		
	case ".png":
		
		imagepng($dst, $target);
		break;
	}
	imagedestroy($dst);
	imagedestroy($src);
	
     chmod ("$dir/$smallFileName", 0755);
     chmod ("$dir/$largeFileName", 0755);
     
	$image->image_file = "/uploaded_images/$largeFileName";
	$image->thumbnail = "/uploaded_images/$smallFileName";
  	
  	
  	return true;
}


$image_id = checkNumeric($_GET["image_id"]);

$image = new Image();
$form = new AutoForm($image);
$form->allowDelete = true;

$imageUpload = new FileUploadFieldRenderer($form, "image_file", "Image File", imageUploadHandler);
$imageUpload->size = 60;
//$form->annotate("image_file", "<i>Upload a large format of the image.<br/>The thumbnail will be generated for you.<br/>The image must be in JPEG format.</i>");

$form->required( "title");
$form->hide("thumbnail");

$imageTypeSelect = new SelectFieldRenderer($form, "image_type", "Image Type");
$imageTypeSelect->allowAddEntry();

$redirect = "images";
$form->button("Cancel", $redirect);

if ($method == "POST")
{
	if ($form->save())
	{
		redirect($redirect);
	}
}

if ($image_id)
{
	$image->load($image_id);
	$title = "Edit Details for {$image->title}";
}
else
{
	$title = "Add a New Image";
}

$script = $form->writeScript();

require_once admin_begin_page;

if ($image_id)
{
?>
<img src="<?echo $image->thumbnail ?> " alt="Small version of image"/><br/>

<?
}
$form->drawForm();

require_once admin_end_page;
?>
