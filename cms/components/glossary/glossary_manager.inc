<?php
/**
 *
 * Provides a central management class for event handlers and 
 * common functionality for the glossary component.
 */

class GlossaryManager
{
	function GlossaryManager()
	{

	}

	static function setDefaults()
	{
		//TODO: Set default configuration parameters here
	}



	static function upgradeComponent($version)
	{
		$mgr = new GlossaryUpgradeManager();
		$mgr->upgrade($version);
	}
}

?>