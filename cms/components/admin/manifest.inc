<?php

class AdminManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "admin";
		$component->description = "Support component for the Fakoli/CMS Administration section";
		$component->author = "Andy Green";
		$component->version = "1.0";
		$component->priority = 0;
		$component->enabled = true;
		
		return $component;
	}
	
	static function getAdminMenu()
	{
		return array("Public Site"			=>	array("Home page" =>	array("page"	=>	"/")));
	}	
}

?>