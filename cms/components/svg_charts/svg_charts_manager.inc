<?php

/**
 *
 * Provides a central management class for event handlers and 
 * common functionality for the svg_charts component.
 */

class SvgChartsManager
{
	function SvgChartsManager()
	{

	}

	static function setDefaults()
	{
		//TODO: Set default configuration parameters here
	}
	
	static function upgradeComponent($version)
	{
		$mgr = new SvgChartsUpgradeManager();
		$mgr->upgrade($version);
	}
	
	static function registerSerializationHandler()
	{
		SerializationManager::registerHandler("svg_charts", "Color Palettes", new SimpleSerializationHandler(ColorPalette));
		return true;
	}
	
	static function outputColorPalettes($script)
	{
		$script .= ColorPalette::generatePalettes();
	}
}

?>