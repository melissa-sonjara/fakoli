<?php

class UserManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "user";
		$component->description = "Standard user management for Fakoli/CMS";
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

				"Users"	=>	array("page"	=>	"/admin/users",
								  "role"	=>	"admin",
								  "weight"	=>	0)
			)
		);		
	}	
}

?>