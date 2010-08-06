<?php
Fakoli::usingFile("framework/tab_bar.inc");

function documentTabs($key)
{
	$tabs = array(	"Details"			=>	"document_form",
					"Classification"	=>	"document_classification",
					"Authors"			=>	"document_authors",
					"Preview"			=>	"document_preview"
					);

	$qs = ($key) ? "document_id=$key" : ""; 
	return new TabBar("tabs", $tabs, $qs);
}
?>