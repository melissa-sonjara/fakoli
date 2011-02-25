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
 
Fakoli::using("attachment", "user");

$attachment = new Attachment();

if (!$_FILES['attachmentFile']) 
{
	die("No upload record found.");
}

if ($_FILES['attachmentFile']["name"]=="") 
{
	die("Upload name is empty");
}

/* Copy across the uploaded file */

trace("Upload Base: {$config['uploadbase']}", 3);
trace("Upload Attachment Directory: {$config['attachmentdir']}", 3);

$dir = $config["attachmentdir"];
$name = $_FILES['attachmentFile']["name"];

trace ("Uploading file to base/file: $base/$file", 3);

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

trace("Upload: _FILES attachmentFile tmp_name is ". $_FILES['attachmentFile']["tmp_name"], 3);

move_uploaded_file($_FILES['attachmentFile']["tmp_name"], "$base/$file");
chmod("$base/$file", 0755);
 
trace("Upload: base/file is $base/$file", 3);

$size =  getScaledSize(filesize("$base/$file"));

$attachment->filename = $name;
$attachment->local_filename = $file;
$attachment->file_size =$size;
$attachment->save();

$icon = getDocIcon($name);

echo "$attachment->attachment_id:$name:$icon:$size";
?>