<?php

class ImageManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "image";
		$component->description = "Image management for Fakoli/CMS";
		$component->author = "Andy Green";
		$component->version = "1.0";
		$component->priority = 0;
		$component->enabled = true;
		
		return $component;
	}
	
	static function getAdminMenu()
	{
		return array
		(
			"Web Site Content"	=> array
			(

				"Images"	=>	array("page"	=>	"/admin/image_galleries",
									  "role"	=>	"admin",
								  	  "weight"	=>	4)
			)
		);		
	}	
}

?>