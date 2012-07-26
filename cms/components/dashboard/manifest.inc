<?php

 
class DashboardManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "dashboard";
		$component->description = "Simple dashboard layout.";
		$component->author = "Andy Green";
		$component->version = "1.0";
		$component->priority = 4;
		$component->enabled = true;
		
		return $component;
	}

	static function getAdminMenu()
	{
		return array
		(	
		);
	}	
}

?>