<?php

class TemplateManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "template";
		$component->description = "Template and CSS administration screens for Fakoli/CMS";
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

				"Templates"	=>	array(	"page"	=>	"/admin/templates",
								  		"role"	=>	"admin",
								  		"weight"	=>	10)
			)
		);		
	}	
}

?>