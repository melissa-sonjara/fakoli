<?php
Fakoli::usingFeature("auto_form");

/**
 *
 * Provides a central management class for event handlers and 
 * common functionality for the date_picker component.
 */
class DatePickerManager
{
	function DatePickerManager()
	{

	}

	static function setDefaults()
	{
		Settings::setDefaultValue("date_picker", "use_mooTools_date_picker", false, Boolean);
		Settings::setDefaultValue("date_picker", "date_picker_theme", "Dashboard", String, "", "", "Bootstrap\nDashboard\njqui\nMinimal\nVista");
	}


	static function registerFieldRenderers()
	{
		global $styles;
		
		if (Settings::getValue("date_picker", "use_mooTools_date_picker"))
		{
			AutoForm::registerFieldRendererClass(DateTime, MooToolsDateTimeFieldRenderer);
		}
	}
	
	static function addTheme($template)
	{
		global $styles;

		$theme = strtolower(Settings::getValue("date_picker", "date_picker_theme"));
		
		$styles .= "<link href='/components/date_picker/css/datepicker.css' rel='stylesheet'/>\n";
		//$styles .= "<link href='/components/date_picker/css/datepicker_{$theme}/datepicker_{$theme}.css' rel='stylesheet'/>\n";
		
		return $template;
	}
}

?>