<?php
Fakoli::using("link_library", "taxonomy");

$link_library_id = checkNumeric($_GET["link_library_id"]);
$link_id = checkNumeric($_GET["link_id"]);

if (!$link_library_id && !$link_id)
{
	Fakoli::end("No link or library specified");
}

if ($link_id)
{
	$link = new LinkRecord($link_id);
	$library = $link->LinkLibrary();
}
else
{
	$link = new LinkRecord();
	$library = new LinkLibrary($link_library_id);
	$link->link_library_id = $link_library_id;
}

if (!$library->allowWriteAccess())
{
	Fakoli::end("Access Denied");
}

$form = new AutoForm($link, "POST", "/action/link_library/link_form?link_id={$link_id}&link_library_id={$link_library_id}");
$form->hide("link_library_id", "owner_id", "last_modified");
$form->ajaxSubmit("function(result) {LinkLibraryManager.linkEdited(result);}", "function() {\$('{$form->id}_error').set('text','Failed to communicate with server'); }");
$form->allowDelete = true;

TaxonomyManager::addTaxonomyFieldRenderers($form);

if ($method == "POST")
{
	if ($form->save())
	{
		Fakoli::end("OK");
	}
	else
	{
		Fakoli::end($form->msg);
	}
}

$script .= $form->writeScript();

$form->drawForm();