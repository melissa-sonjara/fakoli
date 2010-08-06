<?php

class AttachmentManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "attachment";
		$component->description = "Attachment handling for Fakoli/CMS components";
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