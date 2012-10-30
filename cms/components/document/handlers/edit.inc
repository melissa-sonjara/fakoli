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

Fakoli::using("document", "taxonomy");
Fakoli::usingFile("framework/auto_form.inc");

$document_id = checkNumeric($_GET["document_id"]);
if (!$document_id)
{
	die("Invalid Parameter");
}

$document = new Document($document_id);
$library = $document->DocumentLibrary();

if (!checkRole($library->allow_access) && !$library->allowAccess())
{
	die("Access Denied");
}

$document->filter = new InclusionFilter("document_id", "title", "notes", "keywords", "file");

$form = new AutoForm($document, "POST", "/action/document/edit?document_id=$document_id");
$form->readonly("file");
$form->annotate("keywords", "Separate keywords with a comma");
$form->ajaxSubmit("function(result) {editResult(result);}", "function() {\$('{$form->id}_error').set('text','Failed to communicate with server'); }");

$notes = $form->getRenderer("notes");
$notes->colspan = 1;
$notes->columns = 38;

TaxonomyManager::addTaxonomyFieldRenderers($form);

if ($method == "POST")
{
	if ($form->save())
	{
		Fakoli::end("1");
		return;
	}
	else
	{
		Fakoli::end($form->msg);
		return;
	}
}

echo $form->writeScript();
$form->drawForm();
?>