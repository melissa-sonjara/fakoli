<?php

class ImagePickerManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "image_picker";
		$component->description = "Image picker functionality for Fakoli/CMS RTE integration";
		$component->author = "Andy Green";
		$component->version = "1.0";
		$component->priority = 0;
		$component->enabled = true;
		
		return $component;
	}
	
	static function subscribeToEvents()
	{
		return array("Initialize" => array(ImagePickerManifest, onInitialize));
	}	

	
	static function onInitialize()
	{
		global $html_editor_extra_buttons;

		$html_editor_extra_buttons[] =
				array("name"	=>	"image_picker",
					  "image"	=>	"/fakoli/richtext/images/insert_picture.gif", 
					  "tooltip"	=>	"Insert Photos", 
					  "url"		=>	"/image_picker",
					  "width"	=>	"600px",
					  "height"	=>	"670px");
	}
		
	static function getScripts()
	{
		return array("/fakoli/image_picker/image_picker.js");
	}
}

?>