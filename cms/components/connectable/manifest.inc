<?php
/**\defgroup connectable Connectable
 * @{
 */


class ConnectableManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "connectable";
		$component->description = "Implements a free-form relationship engine with navigation";
		$component->author = "Andrew Green";
		$component->version = "1.0";
		$component->priority = 10;
		$component->enabled = true;

		return $component;
	}

	static function getStyles()
	{
		return array(
				//TODO: Add CSS File References "/components/connectable/css/..."
		);
	}

	static function getScripts()
	{
		return array(
				//TODO: Add Javascript File References "/components/connectable/js/..."
		);
	}

	static function subscribeToEvents()
	{
		return array(
					"upgradeComponent"		=>	'ConnectableManager::upgradeComponent',
					"ComponentScanComplete"	=>	'ConnectableManager::setDefaults',
					"Initialize"			=>	'ConnectableManager::onInitialize'
					//TODO: Add Event Registrations here
		);
	}
}

/**@}*/?>