<?php

class PageManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "page";
		$component->description = "Page management for Fakoli/CMS";
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

				"Pages"	=>	array("page"	=>	"/admin/pages",
										  "role"	=>	"admin",
								  		  "weight"	=>	3)
			)
		);		
	}	
	
	static function subscribeToEvents()
	{
		return array("ResolveIdentifier" 	=> array(PageManager, displayPage),
					 "EnumerateItems"		=> array(PageManager, enumeratePages));
	}
}

?>