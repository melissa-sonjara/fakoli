<?
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
					"ComponentScanComplete"				=>	array(APIManager, setDefaults),
					"RegisterSectionContentManagers"	=>	array(APIManager, registerSectionContentManager),
					"upgradeComponent"					=>	array(APIManager, upgradeComponent),
					"DeleteUser"						=>	array(APIManager, deleteUser),
					"RegisterAPIEndPoints"				=>	array(APIManager, registerAPIEndPoints)
					//TODO: Add Event Registrations here
		);
	}
	
	
}

?>