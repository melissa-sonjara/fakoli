<?php

Fakoli::using("document");
Fakoli::usingFile("framework/auto_form.inc");

$document_id = checkNumeric($_GET["document_id"]);
if (!$document_id)
{
	die("Invalid Parameter");
}

$document = new Document($document_id);
$library = $document->DocumentLibrary();

if (!checkRole($library->allow_access))
{
	die("Access Denied");
}

$document->filter = new InclusionFilter("document_id", "title", "notes", "keywords", "file");

$form = new AutoForm($document, "POST", "/action/document/edit?document_id=$document_id");
$form->readonly("file");
$form->ajaxSubmit("function(result) {editResult(result);}", "function() {\$('{$form->id}_error').set('text','Failed to communicate with server'); }");

$notes = $form->getRenderer("notes");
$notes->colspan = 1;
$notes->columns = 38;

if ($method == "POST")
{
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
?>