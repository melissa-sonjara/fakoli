<?php

Fakoli::usingFile("framework/tab_bar.inc");

function documentLibraryTabs($key)
{
	$tabs = array(	"Details"		=>	"/admin/document_library_form",
					"Permissions"	=>	"/admin/document_library_permissions",
					"Files"			=>	"/admin/document_library_files"
					);

	$qs = ($key) ? "document_library_id=$key" : ""; 
	return new TabBar("tabs", $tabs, $qs);
}
?>
