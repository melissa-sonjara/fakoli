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

$mgr = new DocumentManager($library);
$mgr->deleteDocument($document);

echo "1";
return;
?>