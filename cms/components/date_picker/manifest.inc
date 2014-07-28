<?
class DatePickerManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "date_picker";
		$component->description = "Substitutes the more advanced MooTools Date/Time Picker for the standard Fakoli date picker";
		$component->author = "Andrew Green (MooTool DateTime Picker by Arian Stolwijk)";
		$component->version = "1.0";
		$component->priority = 1;
		$component->enabled = true;

		return $component;
	}

	static function getScripts()
	{
		return array(
			"/components/date_picker/js/Locale.en-US.DatePicker.js",
			"/components/date_picker/js/Picker.js",
			"/components/date_picker/js/Picker.Attach.js",
			"/components/date_picker/js/Picker.Date.js"
		);
	}

	static function getStyles()
	{
		return array(
		);
	}
	
	static function subscribeToEvents()
	{
		return array(
					"ComponentScanComplete"	=>	array(DatePickerManager, setDefaults),
					"Initialize"			=>	array(DatePickerManager, registerFieldRenderers)
		);
	}
}

?>