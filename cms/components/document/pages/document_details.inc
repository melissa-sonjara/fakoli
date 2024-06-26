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

Fakoli::using("document", "taxonomy", "comment");
Fakoli::usingFeature("auto_form");

$document_id = checkNumeric($_GET["document_id"]);
if (!$document_id)
{
	redirect("document_libraries");
}

$document = new Document($document_id);
$library = $document->DocumentLibrary();

if (!checkRole($library->allow_access) && !$library->allowAccess())
{
	redirect("document_libraries");
}

$page->page_title = $document->title;
$document->filter = new InclusionFilter("document_id", "notes", "keywords", "author", "file", "publication_date", "publication", "document_library_id");

$form = new AutoForm($document);
$form->data->set("file", DocumentManager::formatDocumentLink($document));
$form->alias("file", "Download");

new RelatedItemSelectFieldRenderer($form, "document_library_id", "Document Library", DocumentLibrary, "WHERE document_library_id={$document->document_library_id}", "<a href='{$library->identifier}'>{name}</a>", "document_library_id");

$form->readOnlyForm = true;
$form->hideEmptyFields = true;

if(checkRole($document->allow_access) && checkRole($library->allow_access))
{
	$form->button("Edit Document", "document_form?document_id={$document_id}");
}

TaxonomyManager::addTaxonomyFieldRenderers($form);

$script .= $form->writeScript();

$form->drawForm();

if($library->allow_comments)
{
	CommentView::addCommentPanel();
}

?>