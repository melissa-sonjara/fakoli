<?php

class EmailManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "email";
		$component->description = "Email management for Fakoli/CMS";
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
			"Classification"	=> array
			(

				"Email Templates"	=>	array("page"	=>	"/admin/email_template_list",
									  "role"	=>	"admin",
									  "weight"	=>	20),
			
				"Merge Codes"	=>	array("page"	=>	"/admin/merge_code_list",
									  "role"	=>	"admin",
									  "weight"	=>	21)
			
			)
		);
	}	
}

?>