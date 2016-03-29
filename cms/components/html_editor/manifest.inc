<?
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
					 "/components/html_editor/js/snippet_tinymce_plugin.js"
		);
	}
	
	static function getStyles()
	{
		return array("/components/html_editor/css/html_editor.css");
	}
	
	static function registerPlugins()
	{
		HtmlEditorManager::registerPlugin("Insert Snippet", "snippet");
	}
	
	static function subscribeToEvents()
	{
		return array("upgradeComponent" 			=> 	array(HtmlEditorManager, upgradeComponent),
					 "ComponentScanComplete"		=>	array(HtmlEditorManager, setDefaults),
					 "Initialize"					=>	array(HtmlEditorManager, onInitialize),
					 "RegisterSerializationHandler"	=>	array(HtmlEditorManager, registerSerializationHandler),
					 "RegisterHTMLEditorPlugins"	=>	array(HtmlEditorManifest, registerPlugins)
					//TODO: Add Event Registrations here
		);
	}
}

?>