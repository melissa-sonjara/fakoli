<?php
/**\defgroup svg Svg
 * @{
 */


class SvgManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "svg";
		$component->description = "Support for embedding svg files directly into CMS pages";
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

	static function subscribeToEvents()
	{
		return array(
					"ComponentScanComplete"	=>	'SvgManager::setDefaults',
					"PostProcessContent"	=>	'SvgManager::substituteTags'
					//TODO: Add Event Registrations here
		);
	}
}

/**@}*/?>