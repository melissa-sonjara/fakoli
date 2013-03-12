<?
class PhonegapManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "phonegap";
		$component->description = "Support for PhoneGap mobile applications";
		$component->author = "Andy Green";
		$component->version = "1.0";
		$component->priority = 1;
		$component->enabled = true;

		return $component;
	}

	static function getScripts()
	{
		return array(
				//TODO: Add Javascript File References "/components/phonegap/js/..."
		);
	}

	static function subscribeToEvents()
	{
		return array(
					"ComponentScanComplete"	=>	array(PhonegapManager, setDefaults),
					"Initialize"			=>	array(PhonegapManager, detectPhonegap),
					"PostProcessScript"		=>	array(PhonegapManager, addScript)
		);
	}
}

?>