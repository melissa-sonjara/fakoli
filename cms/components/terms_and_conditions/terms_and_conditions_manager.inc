<?php
/**
 *
 * Provides a central management class for event handlers and 
 * common functionality for the terms_and_conditions component.
 */

class TermsAndConditionsManager
{
	function TermsAndConditionsManager()
	{

	}

	static function setDefaults()
	{
		Settings::setDefaultValue("terms_and_conditions", "require_acceptance", false, Boolean, "Set to true to require users to accept site terms and conditions", null, null, 1);
		Settings::setDefaultValue("terms_and_conditions", "terms_page_identifier", "terms", String, "The page identifier of the page containing the terms and conditions text", null, null, 3);
		Settings::setDefaultValue("terms_and_conditions", "title", "Terms And Conditions", String, "Title for the popup dialog", null, null, 2);
		Settings::setDefaultValue("terms_and_conditions", "button_label", "", String, "The text to display on the acceptance button", null, null, 4);
	}
}

?>