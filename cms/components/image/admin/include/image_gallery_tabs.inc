<?php

Fakoli::usingFile("framework/tab_bar.inc");

function imageGalleryTabs($key)
{
	$tabs = array(	"Gallery" =>	
					array("Definition"			=>	"/admin/image_gallery_form",
						  "Permissions"			=>	"/admin/image_gallery_permissions"),
					
					"Images" =>	
						array("Thumbnails"		=>	"/admin/images",
							  "Detailed List"	=>	"/admin/image_list")
					);

	$qs = ($key) ? "gallery_id=$key" : ""; 
	return new TwoLevelTabBar($tabs, $qs);
}

function imageGallerySubTabs($key)
{
	
	$tabs = array("Thumbnails"	=>	"/admin/images",
				  "Details"		=>	"/admin/image_list",
			);

	$qs = ($key) ? "gallery_id=$key" : ""; 
	return new TabBar("subtabs", $tabs, $qs);
}
	
?>
