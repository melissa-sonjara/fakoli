<?php
Fakoli::using("document");
$document_library_id = checkNumeric($_GET["document_library_id"]);
$document_ids = checkNumericList($_GET["document_ids"]);

if (!$document_library_id || !$document_ids)
{
	Fakoli::end("Missing Parameter");
}

$library = new DocumentLibrary($document_library_id);
if (!$library->allowManageFiles())
{
	Fakoli::end("Permission Denied");
}

$documentMove = new DocumentMove();
$documentMove->from = $document_library_id;

$form = new AutoForm($documentMove, "POST", "/action/document/move_documents?document_library_id={$document_library_id}&document_ids={$document_ids}");
$form->ajaxSubmit("function(response) { moveDocumentsResponse(response); }");

$form->related(DocumentLibrary, "WHERE document_library_id={$document_library_id}", "{name}", "from", "Move From", false, false);
$form->related(DocumentLibrary, "WHERE document_library_id!={$document_library_id} ORDER BY name", "{name}", "to", "Move To", false, false);
$form->readonly("from");
$form->submitLabel = "Move Documents";
$form->button("Cancel", "ModalDialog.closeAllDialogs(); return true", null, true);

$form->customSaveHandler = function($form) use ($library, $document_ids)
{
	$to = new DocumentLibrary($form->data->to);
	
	$docs = Query::create(Document, "WHERE document_id IN ($document_ids)")->execute();
	
	foreach($docs as $doc)
	{
		$fromPath = $library->getLibraryDirectory().DIRECTORY_SEPARATOR.$doc->file;
		$toPath = $to->getLibraryDirectory().DIRECTORY_SEPARATOR.$doc->file;
		rename($fromPath, $toPath);
		$doc->document_library_id = $to->document_library_id;
		$doc->save();
	}
	return true;
};

if ($method == "POST")
{
	if ($form->save())
	{
		Fakoli::notifyOnNextPage("Documents Moved");
		Fakoli::end("OK");
	}
	else
	{
		Fakoli::end($form->msg);
	}
}

$script .= $form->writeScript();
$form->drawForm();