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

	static function getScripts($ecmascript = false)
	{
		if ($ecmascript)
		{
			return array(
				"/components/connectable/ecmascript/connectable_navigator.js"
			);
		}
		else
		{
			return array(
				"/components/connectable/js/connectable_navigator.js"
			);
		}
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