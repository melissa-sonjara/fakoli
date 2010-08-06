<?php

class FieldRenderersManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "field_renderers";
		$component->description = "Custom Field Renderers used by Fakoli/CMS components";
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