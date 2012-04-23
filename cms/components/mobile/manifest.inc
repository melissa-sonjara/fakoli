<?
class MobileManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "mobile";
		$component->description = "Detects mobile browsers and implements mobile browser content-selection rules.";
		$component->author = "Andy Green";
		$component->version = "1.0";
		$component->priority = 1;
		$component->enabled = true;

		return $component;
	}

	static function subscribeToEvents()
	{
		return array(
					"ComponentScanComplete"	=>	array(MobileManager, setDefaults),
					"Initialize"			=>	array(MobileManager, onInitialize),
					"OverrideTemplate"		=>	array(MobileManager, overrideTemplate)
		);
	}
}

?>