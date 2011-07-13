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

Fakoli::using("document");
Fakoli::usingFile("framework/auto_form.inc");

Fakoli::assertRole("admin");

require_once "include/document_tabs.inc";

$menu_item = "Documents";

$document_id = checkNumeric($_GET["document_id"]);

$document = new Document();

if ($document_id)
{
	$document->load($document_id);
}

$tabs = documentTabs($document_id);

$form = new AutoForm($document);
$form->required("title", "abstract");
$form->hide("is_local", "document_library_id", "owner_id");
$form->allowDelete=true;
$redirect = "admin/documents";
$form->button("Cancel", $redirect);
if ($document->file)
{
	$form->annotate("file", "<a href='/download.php?document_id={$document_id}'>{$document->file}</a>");
}
$fileupload = new FileUploadFieldRenderer($form, "file", "Upload File", uploadPDF);  

if ($method == "POST")
{
	if ($form->save())
	{
		$tabs->queryString = "document_id={$form->data->document_id}";
		$tabs->next();
	} 
}
	
if ($document_id)
{
	$document->load($document_id);
	$title = "Edit Document: {$document->title}";
}
else
{
	$title =  "Add New Document";
}

$script = $form->writeScript();



$tabs->writeHTML();
?>
<div style="clear:left; border: solid 1px #000; width: 100%; margin: 0; padding: 4px;">
<?
$form->drawForm();
?>
</div><br/>
<?



function uploadPDF($field, $document)
{
	global $config;
	
	trace("uploadPDF() called for $field", 3);
	
	if (!$_FILES[$field]) 
	{
		trace("No upload record for $field", 2);
		return false;
	}
	if ($_FILES[$field]["name"]=="") 
	{
		trace("Upload name is empty", 2);
		return false;
	}

  	/* Copy across the uploaded file */

	trace("Upload Base: {$config['uploadbase']}", 3);
	trace("Upload Directory: {$config['uploaddir']}", 3);
	
	$dir = $config["uploaddir"];
	$name = $_FILES[$field]["name"];
	$file = "$dir/$name";
	
	trace ("Uploading file to {$config['uploadbase']}/$file", 3);
	
	if (!file_exists("{$config['uploadbase']}/$dir"))
	{
		// If the directory does not exist, create it 
		mkdir("{$config['uploadbase']}/$dir");
	}
	else if (file_exists("{$config['uploadbase']}/$file"))
	{
		// If a previous copy of the file already exists, remove it
		unlink("{$config['uploadbase']}/$file");
	}

  	move_uploaded_file($_FILES[$field]["tmp_name"], "{$config['uploadbase']}/$file");
  	chmod("{$config['uploadbase']}/$file", 0755);
 
  	$document->file = "$file";
  	
  	return true;
}
?>