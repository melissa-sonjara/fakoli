<?php
require_once realpath(dirname(__FILE__))."/../../framework/tab_bar.inc";

function publicationTabs($key)
{
	$tabs = array(	"Details"			=>	"publication_form.php",
					"Classification"	=>	"publication_classification.php",
					"Authors"			=>	"publication_authors.php",
					"Preview"			=>	"publication_preview.php"
					);

	$qs = ($key) ? "publication_id=$key" : ""; 
	return new TabBar("tabs", $tabs, $qs);
}
?>