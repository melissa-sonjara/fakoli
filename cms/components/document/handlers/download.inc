<?
Fakoli::using("document", "user");


$page_role = "admin,user";

$document_id = checkNumeric($_GET["document_id"]);

$document = new Document($document_id);
$library = $document->DocumentLibrary();

if (!checkRole($library->allow_access))
{
	die("Access Denied");
}

//TODO: Folder level security

$filepath = $library->getLibraryDirectory() . DIRECTORY_SEPARATOR . $document->file;
$mimeType = getMimeType($document->file);
	
$f = fopen($filepath, "r");
$size = filesize($filepath);
	
header("Content-Disposition: attachment;filename=\"" . basename($document->file) . "\"");
header("Content-Type: $mimeType");
header("Content-Length: $size");
	
fpassthru($f);
fclose($f);
?>
