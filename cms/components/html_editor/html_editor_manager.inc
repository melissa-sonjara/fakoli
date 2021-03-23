<?php
/**\addtogroup html_editor
 * @{
 */


/**
 *
 * Provides a central management class for event handlers and 
 * common functionality for the html_editor component.
 */
class HtmlEditorManager
{
	static $plugins = array();			
	static $additionalPlugins = array();
	static $actionFilters = array();
	
	function HtmlEditorManager()
	{

	}

	static function registerPlugin($title, $plugin)
	{
		HtmlEditorManager::$plugins[$plugin] = $title;	
	}
	
	static function registerAdditionalPlugin($title, $plugin)
	{
		HtmlEditorManager::$additionalPlugins[$plugin] = $title;
	}

	static function registerActionFilter($filter)
	{
		HtmlEditorManager::$actionFilters[] = $filter; 
	}
	
	static function upgradeComponent($version)
	{
		$mgr = new HtmlEditorUpgradeManager();
		$mgr->upgrade($version);
	}
	
	static function setDefaults()
	{
		Settings::setDefaultValue("html_editor", "CSS_files", "", Text, "List of CSS files to apply within the editor. Put each CSS file on a separate line.", "Appearance");
		Settings::setDefaultValue("html_editor", "body_class", "", String, "Body class to be applied to content within the editor - use this to override inappropriate styles", "Appearance");

		Settings::setDefaultValue("html_editor", "enable_custom_CSS", false, Boolean, "", "Custom CSS", null, 1);
		Settings::setDefaultValue("html_editor", "custom_CSS", "", Text, "This setting can be used to provide custom CSS styles, configurable from the HTML editor. A link tag to /action/html_editor/custom_styles must be added to your site's template files.", "Custom CSS", null, 2);
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
		ComponentManager::fireEvent("RegisterHTMLEditorActionFilters");
		
		$tiny_mce_application_plugins = implode(",", array_keys(HtmlEditorManager::$plugins));
		$tiny_mce_application_toolbar_section = implode(" ", array_keys(HtmlEditorManager::$plugins)) . " |";
		$tiny_mce_application_context_menu_section = implode(" ", array_keys(HtmlEditorManager::$plugins)) . " |";
		$tiny_mce_content_css = Settings::getValue("html_editor", "CSS_files");
		$tiny_mce_body_class = Settings::getValue("html_editor", "body_class");
		
		if (count(HtmlEditorManager::$actionFilters) > 0)
		{
			HTMLFieldRenderer::$actionFilter = 'HTMLEditorManager::handleActionFilters';
		}
	}
	
	static function handleActionFilters($context)
	{
		foreach(HTMLEditorManager::$actionFilters as $filter)
		{
			$context = $filter($context);
		}
		return $context;
	}
	
	static function applyCustomCSS($template)
	{
		if (Settings::getValue("html_editor", "enable_custom_css"))
		{
			$template = str_replace("</head>", "  <link href='/action/html_editor/custom_css' rel='stylesheet'/>\n</head>", $template);
		}	
		return $template;
	}
	
	static function formatSnippetParameters($snippet)
	{
		$matches = array();
		if (preg_match_all("/{([^}]+)}/", $snippet->snippet, $matches, PREG_PATTERN_ORDER))
		{
			$fields = array_unique($matches[1]);
			
			?>
			<h4>Snippet Parameters</h4>
			<table class='form' id='snippet_parameters'>
			 
			<?
			foreach($fields as $field)
			{
			?>
			<tr><td class='label'><label><?echo $field?></label></td>
			<td class='value'><input type='text' name='<?echo $field?>' class='snippet_parameter' id='snippet_parameter_{$i}' value='' size='75'/></td></tr>
			<?
			}
			?>
			</table>
			<br/>
			<?
		}
	}
	
}

/**@}*/?>