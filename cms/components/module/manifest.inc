<?php

class ModuleManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "module";
		$component->description = "Module management for Fakoli/CMS";
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

				"Modules"	=>	array("page"	=>	"/admin/modules",
									  "role"	=>	"admin",
									  "weight"	=>	2)
			)
		);		
	}	
}

?>