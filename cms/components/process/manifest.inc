<?php
/**\defgroup process Process
 * @{
 */


class ProcessManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "process";
		$component->description = "Provides support for spawning background process and retrieving information about their progress.";
		$component->author = "Andy Green";
		$component->version = "1.0";
		$component->priority = 10;
		$component->enabled = true;

		return $component;
	}

	static function getAdminMenu()
	{
		return array(
				//TODO: Admin Menu Item definitions
		);
	}

	static function getScripts()
	{
		return array(
				"/components/process/js/background_process.js"
		);
	}

	static function getStyles()
	{
		return array(
				"/components/process/css/process.css"
		);
	}
	
	static function subscribeToEvents()
	{
		return array(
					"upgradeComponent"	=>	array(ProcessManager, upgradeComponent),
					"ComponentScanComplete"	=>	array(ProcessManager, setDefaults)
					//TODO: Add Event Registrations here
		);
	}
}

/**@}*/?>