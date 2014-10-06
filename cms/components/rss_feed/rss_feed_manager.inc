<?php
/**
 *
 * Provides a central management class for event handlers and 
 * common functionality for the rss_feed component.
 */

class RssFeedManager
{
	function RssFeedManager()
	{

	}

	static function setDefaults()
	{
		//TODO: Set default configuration parameters here
	}



	static function upgradeComponent($version)
	{
		$mgr = new RssFeedUpgradeManager();
		$mgr->upgrade($version);
	}
}

?>