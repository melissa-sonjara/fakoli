<?php

class CommentManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "comment";
		$component->description = "Commenting functionality for Fakoli/CMS";
		$component->author = "Andy Green";
		$component->version = "1.0";
		$component->priority = 2;
		$component->enabled = true;
		
		return $component;
	}
	
	static function getAdminMenu()
	{
		return array();
	}	
}

?>