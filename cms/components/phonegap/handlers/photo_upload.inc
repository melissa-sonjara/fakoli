<?php
Fakoli::using("image");

$gallery_id = checkNumeric($_GET["gallery_id"]);

trace(print_r($_FILES, true), 2);

if (!$gallery_id)
{
	Fakoli::end("Invalid Parameter");
}

if (!isset($_FILES["photo"]))
{
	Fakoli::end("No photo present in upload");
}

$gallery = new ImageGallery($gallery_id);

if ($gallery->owner_id != $user->get($user->getPrimaryKey()) && !checkRole($gallery->write_access))
{
	Fakoli::end("Permission Denied");
}

//Fudge: Deal with uploads that have no file extension
if (!preg_match("/\\.\\w+$/", $_FILES["photo"]["name"]))
{
	if ($_FILES["photo"]["type"] == "image/jpeg")
	{
		$_FILES["photo"]["name"] .= ".jpg";
	}
	else if ($_FILES["photo"]["type"] == "image/png")
	{
		$_FILES["photo"]["name"] .= ".png";
	}
	else
	{
		Fakoli::end("Unsupported image format");
	}
}

$image = new ImageRecord();
$image->gallery_id = $gallery_id;
$image->title = $_FILES["photo"]["name"];
$image->ALT_tag = "Uploaded Photo";
$image->date_taken = now();
$image->credits = $user->getFullName();
$image->caption = "";
$image->keywords = "";
$image->related_resource = "";
$image->published = false;
$image->include_in_slideshow = false;
$image->sort_order = 0;
$image->image_type = $_FILES["photo"]["type"];
$image->owner_id = $user->get($user->getPrimaryKey());

ImageManager::imageUploadHandler("photo", $image);

$image->save();

Fakoli::end($image->image_id);
