<?php

class ForumManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "forum";
		$component->description = "Discussion forums for Fakoli/CMS";
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
			"Collaboration"	=> array
			(

				"Forums"	=>	array("page"	=>	"/admin/forums",
									  "role"	=>	"admin",
								  	  "weight"	=>	2)
			)
		);		
	}
	
	static function getStyles()
	{
		return array("/fakoli/css/forum.css");
	}
}

?>