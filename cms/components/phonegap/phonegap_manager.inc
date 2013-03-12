<?php
/**
 *
 * Provides a central management class for event handlers and 
 * common functionality for the phonegap component.
 */

class PhonegapManager
{
	function PhonegapManager()
	{

	}

	static function setDefaults()
	{
		Settings::setDefaultValue("phonegap", "enable_phonegap_debugging", false, Boolean, "Enables PhoneGap debugging mode - append phonegap=1 to any URL in your desktop browser to turn on PhoneGap mode");		
	}

	static function detectPhonegap()
	{
		trace(1, "Detecting PhoneGap");
		
		if ($_POST["phonegap"] || stripos($_SERVER['HTTP_USER_AGENT'], "PhoneGap/Cordova") !== false || 
			(Settings::getValue("phonegap", "enable_phonegap_debugging") && $_GET["phonegap"]))
		{
			$_SESSION["phonegap"] = true;
			
			trace(1, "PhoneGap enabled");
		}
	}
	
	static function hasPhonegap()
	{
		return (isset($_SESSION["phonegap"]) && $_SESSION["phonegap"] == true);
	}	
}

?>