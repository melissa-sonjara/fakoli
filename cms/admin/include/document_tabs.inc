<?php
require_once realpath(dirname(__FILE__))."/../../../framework/tab_bar.inc";

function documentTabs($key)
{
	$tabs = array(	"Details"			=>	"document_form.php",
					"Classification"	=>	"document_classification.php",
					"Authors"			=>	"document_authors.php",
					"Preview"			=>	"document_preview.php"
					);

	$qs = ($key) ? "document_id=$key" : ""; 
	return new TabBar("tabs", $tabs, $qs);
}
?>