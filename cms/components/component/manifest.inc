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
								  		  "weight"	=>	0)
			)
		);		
	}	
	
	static function getStyles()
	{
		return array("/fakoli/css/tree.css", 
					 "/fakoli/css/tables.css",
					 "/fakoli/css/dialogs.css",
					 "/fakoli/css/calendar.css",
					 "/fakoli/css/textarea-resizer.css");
	}
	
	static function getScripts()
	{
		return array("/fakoli/js/functions.js",
					 "/fakoli/js/mootools-1.2.4-core.js",
					 "/fakoli/js/mootools-1.2.4-more.js",
					 "/fakoli/js/ui.js",
					 "/fakoli/js/iFrameFormRequest.js",
					 "/fakoli/js/mootools-textarea-resizer.js",
					 "/fakoli/richtext/sonjara_richtext.js",
					 "/fakoli/calendar/sonjara_calendar.js");
	}
}

?>