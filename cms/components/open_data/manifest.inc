<?
class OpenDataManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "open_data";
		$component->description = "Provides support for Open Data compliance and publishing";
		$component->author = "Siobhan Green";
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
					"upgradeComponent"	=>	array(OpenDataManager, upgradeComponent),
					"ComponentScanComplete"	=>	array(OpenDataManager, setDefaults)
					//TODO: Add Event Registrations here
		);
	}
}

?>