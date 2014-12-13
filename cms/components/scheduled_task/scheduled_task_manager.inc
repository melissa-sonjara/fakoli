<?php
/**
 *
 * Provides a central management class for event handlers and 
 * common functionality for the scheduled_task component.
 */

class ScheduledTaskManager
{
	function ScheduledTaskManager()
	{

	}

	static function setDefaults()
	{
		//TODO: Set default configuration parameters here
	}



	static function upgradeComponent($version)
	{
		$mgr = new ScheduledTaskUpgradeManager();
		$mgr->upgrade($version);
	}
}

?>