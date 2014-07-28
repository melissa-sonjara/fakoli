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
		
	}


	static function registerFieldRenderers()
	{
		if (Settings::getValue("date_picker", "use_mooTools_date_picker"))
		{
			AutoForm::registerFieldRendererClass(DateTime, MooToolsDateTimeFieldRenderer);
		}
	}
}

?>