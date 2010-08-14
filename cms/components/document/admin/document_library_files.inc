<?

Fakoli::using("document");

require_once "include/document_library_tabs.inc";

$menu_item = "Documents";

$document_library_id = checkNumeric($_GET["document_library_id"]);

if (!$document_library_id)
{
	redirect("/admin/document_libraries");
}

$library = new DocumentLibrary($document_library_id);

$mgr = new DocumentManager($library);
$tabs = documentLibraryTabs($document_library_id);

$script .= $mgr->writeScript();

$tabs->writeHTML();
?>
<div style="clear:left; border: solid 1px #000; width: 100%; margin: 0; padding: 4px;">
<h3>Files in <?echo $library->name?></h3>
<?
$mgr->drawView();
?>
</div>