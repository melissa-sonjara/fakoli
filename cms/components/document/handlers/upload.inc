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

$document_library_id = checkNumeric($_GET["document_library_id"]);
if (!$document_library_id)
{
	ajaxReturn("Invalid Parameter");
}

global $library;

$library = new DocumentLibrary($document_library_id);

if (!checkRole($library->allow_access))
{
	ajaxReturn("Access Denied");
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
		ajaxReturn("1");
	}
	else
	{
		ajaxReturn($form->msg);
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