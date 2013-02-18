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

Fakoli::using("email", "document");
Fakoli::usingFeature("auto_form");

// We just need any DataItem obj to send to autoform - all fields are additional
$email = new Email();
$email->filter = new InclusionFilter();

$form = new AutoForm($email, "POST", "/action/email/document_select_dialog", "DocumentSelect_form");
$form->ajaxSubmit("function(result) {emailAttachmentMgr.documentAttachmentResult(result);}", "function() {\$('{$form->id}_error').set('text','Failed to communicate with server'); }");
$form->button("Cancel", "emailAttachmentMgr.closeDialog()", null, true);
$form->submitLabel = "Select Document";

$librarySelect = new SelectFieldRenderer($form, "document_library_id", "Library", EmailLibraryAttachmentFieldRenderer::getLibraryOptions());
$documentSelect = new SubSelectFieldRenderer($form, "document_id", "Document", "document_library_id", EmailLibraryAttachmentFieldRenderer::getDocumentOptions(), "{title}");
$form->required("document_id");
// $form->validator->getValidator("document_id, RequiredValidator) is not retrieving validator class here.
// validator needs to set empty to 0 or doesn't validate
$form->validator->add(new RequiredValidator("document_id", "Document", 0, false));

if ($method == "POST")
{
	$form->configureValidators();
	$msg = $form->validator->validate();
	if ($msg == "")
	{
		$id = $_POST["document_id"];
		$document = new Document($id);
		$library = $document->DocumentLibrary();
		$filepath = sanitizePath($library->getLibraryDirectory() . DIRECTORY_SEPARATOR . $document->file);
		$size = filesize($filepath);
		$icon = DocumentHandler::getDocIcon($document->file);
		Fakoli::end("OK|{$document->document_id}|{$library->name}|{$document->title}|{$document->file}|$icon|$size");
	}
	else
	{
		Fakoli::end($msg);
	}
}


echo $form->writeScript();

$form->drawForm();