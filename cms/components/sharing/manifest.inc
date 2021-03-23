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
					"upgradeComponent"					=>	array(SharingManager, upgradeComponent),
					"ComponentScanComplete"				=>	array(SharingManager, setDefaults),
					"RegisterSectionContentManagers"	=>	array(SharingManager, registerSectionContentManager),
					"PostProcessContent"				=>	array(SharingManager, embedSharedItem)
					//TODO: Add Event Registrations here
		);
	}
}

/**@}*/?>