<?php
/**\defgroup mobile_settings Mobile Settings
 * @{
 */


class MobileSettingsManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "mobile_settings";
		$component->description = "Detects mobile browsers and implements mobile browser content-selection rules.";
		$component->author = "Andy Green";
		$component->version = "1.0";
		$component->priority = 1;
		$component->enabled = true;

		return $component;
	}

	var $allow_sessionless_handlers = true;
	
	static function subscribeToEvents()
	{
		return array(
					"ComponentScanComplete"	=>	'MobileSettingsManager::setDefaults',
					"Initialize"			=>	'MobileSettingsManager::onInitialize',
					"OverrideTemplate"		=>	'MobileSettingsManager::overrideTemplate',
					"CheckUseMobile"		=>	'MobileSettingsManager::useMobile'
		);
	}
}

/**@}*/?>