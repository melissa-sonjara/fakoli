<?php
/**\defgroup html_editor Html Editor
 * @{
 */


class HtmlEditorManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "html_editor";
		$component->description = "Allows extensions and configuration for the HTML editor";
		$component->author = "Andy Green";
		$component->version = "1.0";
		$component->priority = 2;
		$component->enabled = true;

		return $component;
	}

	static function getScripts()
	{
		return array("/components/html_editor/js/snippet_manager.js",
					 "/components/html_editor/js/snippet_tinymce_plugin.js",
					 "/components/html_editor/js/custom_css_manager.js",
					 "/components/html_editor/js/custom_css_tinymce_plugin.js",
					 "/components/html_editor/js/tag_editor.js",
					 "/components/html_editor/js/tag_editor_tinymce_plugin.js"
		);
	}
	
	static function getStyles()
	{
		return array("/components/html_editor/css/html_editor.css");
	}
	
	static function registerPlugins()
	{
		HtmlEditorManager::registerPlugin("Insert Snippet", "snippet");
		if (Settings::getValue("html_editor", "enable_custom_CSS") && checkRole("admin"))
		{
			HtmlEditorManager::registerPlugin("Edit Custom CSS", "custom_css");
		}
		HtmlEditorManager::registerPlugin("Edit HTML Tag", "tag_editor");
	}
	
	var $allow_sessionless_handlers = array("custom_css");
	
	static function subscribeToEvents()
	{
		return array("upgradeComponent" 			=> 	'HtmlEditorManager::upgradeComponent',
					 "ComponentScanComplete"		=>	'HtmlEditorManager::setDefaults',
					 "Initialize"					=>	'HtmlEditorManager::onInitialize',
					 "RegisterSerializationHandler"	=>	'HtmlEditorManager::registerSerializationHandler',
					 "RegisterHTMLEditorPlugins"	=>	'HtmlEditorManifest::registerPlugins',
					 "PreProcessPage"				=>	'HtmlEditorManager::applyCustomCSS'
					//TODO: Add Event Registrations here
		);
	}
}

/**@}*/?>