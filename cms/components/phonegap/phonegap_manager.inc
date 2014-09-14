<?php
/**
 *
 * Provides a central management class for event handlers and 
 * common functionality for the phonegap component.
 */

global $phonegap;

class PhonegapManager
{
	static $phonegapFiles = array();
	
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
		trace("Detecting PhoneGap", 4);
		
		if ($_SESSION["phonegap"]) return;
		
		$matches = array();
		if (preg_match("/PhoneGap\\/Cordova (\\w+) (\\d+\\.\\d+\\.\\d+)/", $_SERVER['HTTP_USER_AGENT'], $matches))
		{
			$_SESSION["phonegap"] = true;	
			$_SESSION["phonegap_os"] = $matches[1];
			$_SESSION["phonegap_version"] = $matches[2];

			trace("PhoneGap {$_SESSION["phonegap_version"]} enabled via User Agent", 3);
			return;
		}
					
		if ($_POST["phonegap"])
		{
			$_SESSION["phonegap"] = true;
			$_SESSION["phonegap_version"] = $_POST["phonegap_version"];
			$_SESSION["phonegap_os"] = $_POST["phonegap_os"];
			
			trace("PhoneGap {$_SESSION["phonegap_version"]} enabled via POST", 3);
			return;
		}	
		
		if (Settings::getValue("phonegap", "enable_phonegap_debugging") && $_GET["phonegap"])
		{
			$_SESSION["phonegap"] = true;
			$_SESSION["phonegap_version"] = Settings::getValue("phonegap", "phonegap_debug_version");
			$_SESSION["phonegap_os"] = Settings::getValue("phonegap", "phonegap_debug_os");
			
			trace("PhoneGap {$_SESSION["phonegap_version"]} enabled in debugging mode", 3);
		}
	}
	
	static function hasPhonegap()
	{
		return (isset($_SESSION["phonegap"]) && $_SESSION["phonegap"] == true) || stripos($_SERVER['HTTP_USER_AGENT'], "PhoneGap/Cordova") !== false;
	}	
	
	static function showVersion()
	{
		if ($_SESSION["phonegap"])
		{
			echo "<p>Using Phonegap version ".$_SESSION["phonegap_version"]." on ".$_SESSION["phonegap_os"]."</p>";
		}
		else
		{
			echo "<p>Not using Phonegap</p>";
		}
	}
	
	static function addScript($script)
	{
		global $phonegap;
		
		if (PhonegapManager::hasPhonegap())
		{
			ComponentManager::fireEvent("RegisterPhonegapFiles");
			
			$file = PhonegapManager::getPhonegapFile($_SESSION["phonegap_os"], $_SESSION["phonegap_version"]);
			
			$phonegap = "<script type='text/javascript' src='{$file}'></script>\n".$phonegap;
		}
		else
		{
			$phonegap = "";
		}
		
		return $script;
	}
	
	static function registerPhonegapFile($platform, $version, $file)
	{
		PhonegapManager::$phonegapFiles["{$platform}-{$version}"] = $file;
	}
	
	static function getPhonegapFile($platform, $version)
	{
		$key = "{$platform}-{$version}";
		if (array_key_exists($key, PhonegapManager::$phonegapFiles))
		{
			// Send the registered configured file for the platform and version
			return PhonegapManager::$phonegapFiles[$key];
		}
		
		// Send default file with no plugins
		return "/components/phonegap/js/{$platform}/cordova-{$version}.js";
	}
}

?>