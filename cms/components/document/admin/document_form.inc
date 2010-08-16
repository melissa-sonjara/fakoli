<?php

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
$form->hide("is_local");
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