<?php
/**
 *
 * Provides a central management class for event handlers and 
 * common functionality for the bookmark component.
 */

class BookmarkManager
{
	function BookmarkManager()
	{

	}

	static function setDefaults()
	{
		//TODO: Set default configuration parameters here
	}



	static function upgradeComponent($version)
	{
		$mgr = new BookmarkUpgradeManager();
		$mgr->upgrade($version);
	}
}

?>