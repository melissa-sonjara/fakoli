<?php

class SiteManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "site";
		$component->description = "";
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

				"Sites"	=>	array("page"	=>	"/admin/sites",
								  "role"	=>	"admin",
								  "weight"	=>	1)
			)
		);		
	}	
}

?>