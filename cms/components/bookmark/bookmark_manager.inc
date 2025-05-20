<?php
/**\addtogroup bookmark
 * @{
 */


/**
 *
 * Provides a central management class for event handlers and 
 * common functionality for the bookmark component.
 */

class BookmarkManager
{
	function __construct()
	{

	}

	static function setDefaults()
	{
		Settings::setDefaultValue("bookmark", "show_url", true, Boolean, "Specifies whether to show the URL in the bookmark popup window");
	}



	static function upgradeComponent($version)
	{
		$mgr = new BookmarkUpgradeManager();
		$mgr->upgrade($version);
	}
}

/**@}*/?>
