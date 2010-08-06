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
		return null;
	}	
}

?>