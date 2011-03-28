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

Fakoli::using("document", "fileshare");
Fakoli::usingFile("framework/auto_form.inc");

$document_id = checkNumeric($_GET["document_id"]);
if (!$document_id)
{
	ajaxReturn("Invalid Parameter");
}

$document = new Document($document_id);
$library = $document->DocumentLibrary();

if (!checkRole($library->allow_access) && !$library->allowAccess())
{
	ajaxReturn("Access Denied");
}

$document->filter = new InclusionFilter("document_id", "title", "notes", "keywords", "file", "author");

$form = new AutoForm($document, "POST", "/action/fileshare/edit?document_id=$document_id", "DocumentDetails_form");
$form->readonly("file");
$form->required("title");
$form->ajaxSubmit("function(result) {new LibraryManager().editDocumentDetailsResult(result);}", "function() {\$('{$form->id}_error').set('text','Failed to communicate with server'); }");
$form->button("Cancel", "new LibraryManager().closeEditDocumentDetails()", null, true);

$notes = $form->getRenderer("notes");
$notes->colspan = 1;
$notes->columns = 38;

if ($method == "POST")
{
	if ($form->save())
	{
		ajaxReturn("OK");
		return;
	}
	else
	{
		ajaxReturn($form->msg);
		return;
	}
}

echo $form->writeScript();
$form->drawForm();
?>