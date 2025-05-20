<?php
/**\addtogroup open_data
 * @{
 */


/**
 *
 * Provides a central management class for event handlers and 
 * common functionality for the open_data component.
 */

class OpenDataManager
{
	function __construct()
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

	static function registerSerializationHandler()
	{
		SerializationManager::registerHandler("open_data", "Open Data Definitions", new SimpleSerializationHandler(OpenData));
		return true;
	}
}

/**@}*/?>
