<?php
Fakoli::using("attachment", "user");

$attachment = new Attachment();

if (!$_FILES['attachmentFile']) 
{
	die("No upload record found.");
}

if ($_FILES[attachmentFile]["name"]=="") 
{
	die("Upload name is empty");
}

/* Copy across the uploaded file */

trace("Upload Base: {$config['uploadbase']}", 3);
trace("Upload Directory: {$config['uploaddir']}", 3);

$dir = $config["attachmentdir"];
$name = $_FILES['attachmentFile']["name"];
$base = $config['uploadbase'];


trace ("Uploading file to $base/$file", 3);

if (!file_exists("$base/$dir"))
{
	// If the directory does not exist, create it 
	mkdir("{$config['uploadbase']}/$dir");
}

$attachment->user_id = $user->get($user->primary_key);
$attachment->save();

$extension = substr($name, strrpos($name, "."));

$file = "$dir/{$attachment->attachment_id}$extension";

if (file_exists("$base/$file"))
{
	// If a previous copy of the file already exists, remove it
	unlink("$base/$file");
}

move_uploaded_file($_FILES['attachmentFile']["tmp_name"], "$base/$file");
chmod("$base/$file", 0755);
 
$size =  getScaledSize(filesize("$base/$file"));

$attachment->filename = $name;
$attachment->local_filename = $file;
$attachment->file_size =$size;
$attachment->save();

$icon = getDocIcon($name);

echo "$attachment->attachment_id:$name:$icon:$size";
?>