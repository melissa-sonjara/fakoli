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



	static function upgradeComponent($version)
	{
		$mgr = new OpenDataUpgradeManager();
		$mgr->upgrade($version);
	}
}

?>