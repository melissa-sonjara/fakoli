<?
class SessionExtenderManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "session_extender";
		$component->description = "";
		$component->author = "";
		$component->version = "1.0";
		$component->priority = 10;
		$component->enabled = true;

		return $component;
	}

	static function getScripts()
	{
		return array(
				//TODO: Add Javascript File References "/components/session_extender/js/..."
		);
	}

	static function subscribeToEvents()
	{
		return array(
					"ComponentScanComplete"	=>	array(SessionExtenderManager, setDefaults)
					//TODO: Add Event Registrations here
		);
	}
}

?>