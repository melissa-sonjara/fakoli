<?php

class LinkPickerManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "link_picker";
		$component->description = "Link picker functionality for Fakoli/CMS RTE integration";
		$component->author = "Andy Green";
		$component->version = "1.0";
		$component->priority = 0;
		$component->enabled = true;
		
		return $component;
	}
	
	static function subscribeToEvents()
	{
		return array("Initialize" => array(LinkPickerManifest, onInitialize));
	}	

	
	static function onInitialize()
	{
		global $html_editor_extra_buttons;

		$html_editor_extra_buttons[] =
				array("name"	=>	"link_picker",
					  "image"	=>	"/fakoli/richtext/images/document_link.gif", 
					  "tooltip"	=>	"Link to Page or Document", 
					  "url"		=>	"/link_picker",
					  "width"	=>	"500px",
					  "height"	=>	"600px");
	}
		
}

?>