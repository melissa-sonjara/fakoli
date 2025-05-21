<?php
/**\defgroup api Api
 * @{
 */


class ApiManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "api";
		$component->description = "Support for providing remote API hooks for Fakoli components";
		$component->author = "Melissa Wilson";
		$component->version = "1.0";
		$component->priority = 10;
		$component->enabled = true;

		return $component;
	}

	static function subscribeToEvents()
	{
		return array(
					"ComponentScanComplete"				=>	'APIManager::setDefaults',
					"RegisterSectionContentManagers"	=>	'APIManager::registerSectionContentManager',
					"upgradeComponent"					=>	'APIManager::upgradeComponent',
					"DeleteUser"						=>	'APIManager::deleteUser',
					"RegisterAPIEndPoints"				=>	'APIManager::registerAPIEndPoints'
					//TODO: Add Event Registrations here
		);
	}
	
	
}

/**@}*/?>