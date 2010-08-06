<?php

class LoginManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "login";
		$component->description = "Standard login component for Fakoli/CMS";
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