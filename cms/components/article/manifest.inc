<?php

class ArticleManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "article";
		$component->description = "Article management for Fakoli/CMS";
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

				"Articles"	=>	array("page"	=>	"/admin/articles",
								  	  "role"	=>	"admin",
								  	  "weight"	=>	5)
			)
		);		
	}	
}

?>