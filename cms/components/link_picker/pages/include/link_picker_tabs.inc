<?php
Fakoli::usingFeature("tab_bar");

function linkPickerTabs($qs)
{
	$tabs = array(	"Pages"		=>	"/link_picker",
					"Documents"	=>	"/document_picker",
					"Articles"	=>	"/article_picker"
					);

	$bar = new TabBar("tabs", $tabs, $qs);
	
	return $bar;
}
?>