<?php

class ComponentManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "component";
		$component->description = "Component management for Fakoli/CMS";
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

				"Components"	=>	array("page"	=>	"/admin/components",
										  "role"	=>	"admin",
								  		  "weight"	=>	2)
			)
		);		
	}	
}

?>