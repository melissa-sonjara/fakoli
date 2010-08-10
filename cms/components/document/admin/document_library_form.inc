<?php
Fakoli::using("document", "role");
Fakoli::usingFile("framework/auto_form.inc");

require_once "include/document_library_tabs.inc";

$document_library_id = checkNumeric($_GET["document_library_id"]);

if ($document_library_id)
{
	$library = new DocumentLibrary($document_library_id);
}
else
{
	$library = new DocumentLibrary();
	$library->owner_id = $user->user_id;
}

$library->filter = new InclusionFilter("document_library_id", "name", "description", "owner_id", "last_modified");

$tabs = documentLibraryTabs($document_library_id);

$form = new AutoForm($library);
$form->hide("owner_id");

if ($method == "POST")
{
	if ($form->save())
	{
		$d = $form->data->getLibraryDirectory();
		if (!is_dir($d) && !file_exists($d))
		{
			mkdir($d);
		}
		
		$tabs->queryString = "document_library_id=".$form->data->document_library_id;
		$tabs->next();
	}
}


$script .= $form->writeScript();

$tabs->writeHTML();
?>
<div style="clear:left; border: solid 1px #000; width: 100%; margin: 0; padding: 4px;">
<?
$form->drawForm();
?>
</div>