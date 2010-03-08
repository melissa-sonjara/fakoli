<?
$page_role = "admin,user";

require_once "config.php";
require_once "datamodel/publication.inc";

$publication_id = checkNumeric($_GET["publication_id"]);
$publication = new Publication($publication_id);

if ($publication->pdf_file)
{
	$filepath = "{$config['uploadbase']}/{$publication->pdf_file}";
	$mimeType = getMimeType($publication->pdf_file);
	
	$f = fopen($filepath, "r");
	$size = filesize($filepath);
	
	header("Content-Disposition: attachment;filename=\"" . basename($publication->pdf_file) . "\"");
	header("Content-Type: $mimeType");
	header("Content-Length: $size");
	
	fpassthru($f);
	fclose($f);
}	
?>
