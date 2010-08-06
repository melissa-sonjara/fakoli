<?php

class MenuManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "menu";
		$component->description = "Menu management for Fakoli/CMS";
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

				"Menus"	=>	array("page"	=>	"/admin/menus",
								  "role"	=>	"admin",
								  "weight"	=>	2)
			)
		);		
	}	
}

?>