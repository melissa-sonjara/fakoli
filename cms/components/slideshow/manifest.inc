<?php

class SlideshowManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "slideshow";
		$component->description = "Zoomable slideshow component for uploaded images";
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