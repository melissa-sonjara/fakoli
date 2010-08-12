<?php

Fakoli::using("document");
Fakoli::usingFile("framework/auto_form.inc");

$document_library_id = checkNumeric($_GET["document_library_id"]);
if (!$document_library_id)
{
	die("Invalid Parameter");
}

global $library;

$library = new DocumentLibrary($document_library_id);

if (!checkRole($library->allow_access))
{
	die("Access Denied");
}

$document = new Document();

$document->filter = new InclusionFilter("document_id", "title", "notes", "keywords", "file", "publication_date", "document_library_id", "owner_id");

$form = new AutoForm($document, "POST", "/action/document/upload?document_library_id=$document_library_id");
$form->ajaxSubmit("function(result) {uploadResult(result);}", "function() {\$('{$form->id}_error').set('text','Failed to communicate with server'); }");
$form->hide("publication_date", "document_library_id", "owner_id");

$notes = $form->getRenderer("notes");
$notes->colspan = 1;
$notes->columns = 38;

$folderSelect = new FolderSelectFieldRenderer($form, "folder", "Folder", $library);
$fileUpload = new FileUploadFieldRenderer($form, "file", "File", uploadLibraryFile);

if ($method == "POST")
{
	$_POST["publication_date"] = now();
	$_POST["owner_id"] = $user->user_id;
	$_POST["document_library_id"] = $document_library_id;
	
	if ($form->save())
	{
		echo "1";
		return;
	}
	else
	{
		echo $form->msg;
		return;
	}
}

echo $form->writeScript();
$form->drawForm();

function uploadLibraryFile($field, $document)
{
	global $library;
	global $form;
	
	$mgr = new DocumentManager($library);
	$document->file = $mgr->uploadFile($field, $_POST["folder_value"]);
}
?>