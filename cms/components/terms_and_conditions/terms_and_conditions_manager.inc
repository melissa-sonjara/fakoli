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
		Settings::setDefaultValue("terms_and_conditions", "require_acceptance", false, Boolean, "Set to true to require users to accept site terms and conditions");
		Settings::setDefaultValue("terms_and_conditions", "terms_page_identifier", "terms", String, "The page identifier of the page containing the terms and conditions text");
		Settings::setDefaultValue("terms_and_conditions", "title", "Terms And Conditions", String);
	}
}

?>