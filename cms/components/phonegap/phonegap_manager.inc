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
		Settings::setDefaultValue("phonegap", "phonegap_debug_version", "2.4.0", String, "The version of Phonegap that is assumed in debugging mode");
		Settings::setDefaultValue("phonegap", "phonegap_debug_os", "android", String, "The OS of Phonegap that is assumed in debugging mode");
		
	}

	static function detectPhonegap()
	{
		global $script;
		trace(1, "Detecting PhoneGap");
		
		if ($_SESSION["phonegap"]) return;
		
		$matches = array();
		if (preg_match("/PhoneGap\\/Cordova (\\w+) (\\d+\\.\\d+\\.\\d+)/", $_SERVER['HTTP_USER_AGENT'], $matches))
		{
			$_SESSION["phonegap"] = true;	
			$_SESSION["phonegap_os"] = $matches[1];
			$_SESSION["phonegap_version"] = $matches[2];

			trace(1, "PhoneGap {$_SESSION["phonegap_version"]} enabled via User Agent");
			return;
		}
					
		if ($_POST["phonegap"])
		{
			$_SESSION["phonegap"] = true;
			$_SESSION["phonegap_version"] = $_POST["phonegap_version"];
			$_SESSION["phonegap_os"] = $_POST["phonegap_os"];
			
			trace(1, "PhoneGap {$_SESSION["phonegap_version"]} enabled via POST");
			return;
		}	
		
		if (Settings::getValue("phonegap", "enable_phonegap_debugging") && $_GET["phonegap"])
		{
			$_SESSION["phonegap"] = true;
			$_SESSION["phonegap_version"] = Settings::getValue("phonegap", "phonegap_debug_version");
			$_SESSION["phonegap_os"] = Settings::getValue("phonegap", "phonegap_debug_os");
			
			trace(1, "PhoneGap {$_SESSION["phonegap_version"]} enabled in debugging mode");
		}
	}
	
	static function hasPhonegap()
	{
		return (isset($_SESSION["phonegap"]) && $_SESSION["phonegap"] == true) || stripos($_SERVER['HTTP_USER_AGENT'], "PhoneGap/Cordova") !== false;
	}	
	
	static function addScript($script)
	{
		global $phonegap;
		
		if (PhonegapManager::hasPhonegap())
		{
			$script .= "<script type='text/javascript' src='/components/phonegap/js/{$_SESSION["phonegap_os"]}/cordova-{$_SESSION["phonegap_version"]}.js'></script>\n";
			$script .= $phonegap;
		}
		
		return $script;
	}
}

?>