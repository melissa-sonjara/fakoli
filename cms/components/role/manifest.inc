<?php

class RoleManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "role";
		$component->description = "Role management for Fakoli/CMS";
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
			"User Management"	=> array
			(

				"Roles"	=>	array("page"	=>	"/admin/roles",
										  "role"	=>	"admin",
								  		  "weight"	=>	2)
			)
		);		
	}	
}

?>