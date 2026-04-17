<?php
/**\defgroup date_picker Date Picker
 * @{
 */


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

	static function getScripts($ecmascript = false)
	{
		if ($ecmascript)
		{
			return array(
				"/components/date_picker/ecmascript/Locale.en-US.DatePicker.js",
				"/components/date_picker/ecmascript/Locale.af-ZA.DatePicker.js",
				"/components/date_picker/ecmascript/Locale.cs-CZ.DatePicker.js",
				"/components/date_picker/ecmascript/Locale.de-DE.DatePicker.js",
				"/components/date_picker/ecmascript/Locale.es-ES-DatePicker.js",
				"/components/date_picker/ecmascript/Locale.fr-FR.DatePicker.js",
				"/components/date_picker/ecmascript/Locale.he-IL.DatePicker.js",
				"/components/date_picker/ecmascript/Locale.hu-HU.DatePicker.js",
				"/components/date_picker/ecmascript/Locale.it-IT.DatePicker.js",
				"/components/date_picker/ecmascript/Locale.nl-NL.DatePicker.js",
				"/components/date_picker/ecmascript/Locale.pl-PL.DatePicker.js",
				"/components/date_picker/ecmascript/Locale.pt-BR.DatePicker.js",
				"/components/date_picker/ecmascript/Locale.ru-RU.DatePicker.js",
				"/components/date_picker/ecmascript/Locale.sv-SE.DatePicker.js",
				"/components/date_picker/ecmascript/Locale.uk-UA.DatePicker.js",
				"/components/date_picker/ecmascript/Picker.js",
				"/components/date_picker/ecmascript/Picker.Attach.js",
				"/components/date_picker/ecmascript/Picker.Date.js",
				"/components/date_picker/ecmascript/Picker.Date.Range.js"
			);
		}
		else
		{
			return array(
				"/components/date_picker/js/Locale.en-US.DatePicker.js",
				"/components/date_picker/js/Locale.af-ZA.DatePicker.js",
				"/components/date_picker/js/Locale.cs-CZ.DatePicker.js",
				"/components/date_picker/js/Locale.de-DE.DatePicker.js",
				"/components/date_picker/js/Locale.es-ES-DatePicker.js",
				"/components/date_picker/js/Locale.fr-FR.DatePicker.js",
				"/components/date_picker/js/Locale.he-IL.DatePicker.js",
				"/components/date_picker/js/Locale.hu-HU.DatePicker.js",
				"/components/date_picker/js/Locale.it-IT.DatePicker.js",
				"/components/date_picker/js/Locale.nl-NL.DatePicker.js",
				"/components/date_picker/js/Locale.pl-PL.DatePicker.js",
				"/components/date_picker/js/Locale.pt-BR.DatePicker.js",
				"/components/date_picker/js/Locale.ru-RU.DatePicker.js",
				"/components/date_picker/js/Locale.sv-SE.DatePicker.js",
				"/components/date_picker/js/Locale.uk-UA.DatePicker.js",
				"/components/date_picker/js/Picker.js",
				"/components/date_picker/js/Picker.Attach.js",
				"/components/date_picker/js/Picker.Date.js",
				"/components/date_picker/js/Picker.Date.Range.js"
			);
		}
	}

	static function getStyles()
	{
		return array(
		);
	}
	
	static function subscribeToEvents()
	{
		return array(
					"ComponentScanComplete"	=>	'DatePickerManager::setDefaults',
					"Initialize"			=>	'DatePickerManager::registerFieldRenderers',
					"PreProcessPage"		=>	'DatePickerManager::addTheme'
		);
	}
}

/**@}*/?>