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

	static function upgradeComponent($version)
	{
		$mgr = new HtmlEditorUpgradeManager();
		$mgr->upgrade($version);
	}
	
	static function setDefaults()
	{
		Settings::setDefaultValue("html_editor", "CSS_files", "", String, "Comma-separated list of CSS files to apply within the editor", "Appearance");
		Settings::setDefaultValue("html_editor", "body_class", "", String, "Body class to be applied to content within the editor - use this to override inappropriate styles", "Appearance");
	}

	static function registerSerializationHandler()
	{
		SerializationManager::registerHandler("snippets", "Snippets", new SimpleSerializationHandler(Snippet));
		return true;
	}
	
	static function onInitialize()
	{
		global $tiny_mce_application_plugins;
		global $tiny_mce_application_toolbar_section;
		global $tiny_mce_application_context_menu_section;
		global $tiny_mce_content_css;
		global $tiny_mce_body_class;
		
		ComponentManager::fireEvent("RegisterHTMLEditorPlugins");
		
		$tiny_mce_application_plugins = implode(",", array_keys(HtmlEditorManager::$plugins));
		$tiny_mce_application_toolbar_section = implode(" ", array_keys(HtmlEditorManager::$plugins)) . " |";
		$tiny_mce_application_context_menu_section = implode(" ", array_keys(HtmlEditorManager::$plugins)) . " |";
		$tiny_mce_content_css = Settings::getValue("html_editor", "CSS_files");
		$tiny_mce_body_class = Settings::getValue("html_editor", "body_class");
	}
}

?>