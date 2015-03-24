<?php
/**
 *
 * Provides a central management class for event handlers and 
 * common functionality for the open_data component.
 */

class OpenDataManager
{
	function OpenDataManager()
	{

	}

	static function setDefaults()
	{
		//TODO: Set default configuration parameters here
	}

	static function getOpenDataClasses()
	{
		$classes = array();
		$classes = ComponentManager::fireEvent("EnumerateOpenDataClasses");
		return $classes;
	}

	static function upgradeComponent($version)
	{
		$mgr = new OpenDataUpgradeManager();
		$mgr->upgrade($version);
	}




	function Ignored($openData)
	{
		$img = $openData->ignored ?  "off.png": "on.png" ;
		$alt = $openData->ignored ? "Ignored" : "Used";
	
		return "<img src='/fakoli/images/$img' alt='$alt' style='display: inline-block; vertical-align: middle; border: none; cursor: pointer'/>";
	}

}

?>