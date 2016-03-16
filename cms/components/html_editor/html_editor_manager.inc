<?php
/**
 *
 * Provides a central management class for event handlers and 
 * common functionality for the html_editor component.
 */

class HtmlEditorManager
{
	static $plugins = array();
	
	function HtmlEditorManager()
	{

	}

	static function registerPlugin($title, $plugin)
	{
		HtmlEditorManager::$plugins[$plugin] = $title;	
	}
	
	static function setDefaults()
	{
		//TODO: Set default configuration parameters here
	}

	static function onInitialize()
	{
		global $tiny_mce_application_plugins;
		global $tiny_mce_application_toolbar_section;
		global $tiny_mce_application_context_menu_section;
		
		ComponentManager::fireEvent("RegisterHTMLEditorPlugins");
		
		$tiny_mce_application_plugins = implode(",", array_keys(HtmlEditorManager::$plugins));
		$tiny_mce_application_toolbar_section = implode(" ", array_keys(HtmlEditorManager::$plugins)) . " |";
		$tiny_mce_application_context_menu_section = implode(" ", array_keys(HtmlEditorManager::$plugins)) . " |";		
	}
}

?>