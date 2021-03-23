<?php
/**\addtogroup svg
 * @{
 */


/**
 *
 * Provides a central management class for event handlers and 
 * common functionality for the svg component.
 */

class SvgManager
{
	function SvgManager()
	{

	}

	static function setDefaults()
	{
		Settings::setDefaultValue("svg", "svg_directory", "/templates/svg/", String, "The directory that contains your SVG assets");
	}

	static function substituteTags($content)
	{
		global $config;
		preg_match_all("/\\[svg:(.*?)\\]/", $content, $matches, PREG_PATTERN_ORDER);	
		$dir = Settings::getValue("svg", "svg_directory");
		
		foreach($matches[1] as $file)
		{
			$c = file_get_contents($config['homedir'] . $dir . $file);
			
			$content = str_replace("[svg:$file]", $c, $content);
		}
		
		return $content;
	}

}

/**@}*/?>