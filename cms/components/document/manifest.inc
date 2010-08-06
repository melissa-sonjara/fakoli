<?php

class DocumentManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "document";
		$component->description = "Document management for Fakoli/CMS";
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

				"Documents"	=>	array("page"	=>	"/admin/documents",
									  "role"	=>	"admin",
									  "weight"	=>	3)
			)
		);		
	}	
}

?>