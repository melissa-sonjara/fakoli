<?php
Fakoli::using("image");

$gallery_id = checkNumeric($_GET["gallery_id"]);

if (!$gallery_id)
{
	Fakoli::end("Invalid Parameter");
}

if (!isset($_FILES["photo"]))
{
	Fakoli::end("No photo present in upload");
}

$gallery = new ImageGallery($gallery_id);

if ($owner_id != $user->get($user->getPrimaryKey()))
{
	Fakoli::end("Permission Denied");
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

ImageManager::imageUploadHandlers("photo", $image);

$image->save();

Fakoli::end($image->image_id);
