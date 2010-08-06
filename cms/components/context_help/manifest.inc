<?php

class ContextHelpManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "context_help";
		$component->description = "Context Help system for providing inline help in Fakoli forms";
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

				"Context Help"			=>	array("page"	=>	"/admin/context_help_list",
											  "role"	=>	"admin",
											  "weight"	=>	3))	
		);		
				
	}	
}

?>