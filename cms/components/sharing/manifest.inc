<?php
/**\defgroup sharing Sharing
 * @{
 */


class SharingManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "sharing";
		$component->description = "Provides a mechanism for sharing work products without a login";
		$component->author = "Andy Green";
		$component->version = "1.0";
		$component->priority = 20;
		$component->enabled = true;

		return $component;
	}

	static function subscribeToEvents()
	{
		return array(
					"upgradeComponent"					=>	'SharingManager::upgradeComponent',
					"ComponentScanComplete"				=>	'SharingManager::setDefaults',
					"RegisterSectionContentManagers"	=>	'SharingManager::registerSectionContentManager',
					"PostProcessContent"				=>	'SharingManager::embedSharedItem'
					//TODO: Add Event Registrations here
		);
	}
}

/**@}*/?>