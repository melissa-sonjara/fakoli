<?php

class ClassificationManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "classification";
		$component->description = "Basic Classification functionality for Fakoli/CMS";
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
			"Classification"	=> array(

				"Groups"			=>	array("page"	=>	"/admin/groups",
											  "role"	=>	"admin",
											  "weight"	=>	0),
	
				"Topics"			=>	array("page"	=>	"/admin/topics",
											  "role"	=>	"admin",
											  "weight"	=>	1)),
		);		
				
	}	
}

?>