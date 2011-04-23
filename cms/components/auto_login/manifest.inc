<?php


class AutoLoginManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "auto_login";
		$component->description = "Automatic login via pre-shared tokens. Useful for authenticating scheduled tasks, etc.";
		$component->author = "Andy Green";
		$component->version = "1.0";
		$component->priority = 5;
		$component->enabled = true;
		
		return $component;
	}
	
	static function getAdminMenu()
	{
			return array("User Management"	=> array
			(

				"Authentication Tokens"=>	array("page"	=>	"/admin/authentication_tokens",
										  "role"	=>	"admin",
								  		  "weight"	=>	1
										)
			)
		);		
	}	
}

?>