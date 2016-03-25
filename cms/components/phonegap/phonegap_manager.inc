<?php
/**
 *
 * Provides a central management class for event handlers and 
 * common functionality for the phonegap component.
 */

global $phonegap;

/**
 * Provides support for managing Phonegap apps.
 * @author andy
 */
class PhonegapManager
{
	static $phonegapFiles = array();
	static $appScripts = array();
	static $supportScripts = array();
	
	function PhonegapManager()
	{

	}

	static function setDefaults()
	{
		Settings::setDefaultValue("phonegap", "enable_phonegap", true, Boolean, "Enables support for Phonegap/Cordova applications");
		Settings::setDefaultValue("phonegap", "enable_phonegap_debugging", false, Boolean, "Enables PhoneGap debugging mode - append phonegap=1 to any URL in your desktop browser to turn on PhoneGap mode");		
		Settings::setDefaultValue("phonegap", "phonegap_debug_version", "2.4.0", String, "The version of Phonegap that is assumed in debugging mode");
		Settings::setDefaultValue("phonegap", "phonegap_debug_os", "android", String, "The OS of Phonegap that is assumed in debugging mode");
		Settings::setDefaultValue("phonegap", "use_local_phonegap_files", false, Boolean, "Check this box to pull the local cordova files in the packaged app, instead of pulling them from the server (EXPERIMENTAL)");
	}

	/**
	 * Detects whether a phonegap / cordova app is being used to access the current page.
	 */
	static function detectPhonegap()
	{
		global $script;
		
		trace("Detecting PhoneGap", 4);
		
		if ($_SESSION["phonegap"]) 
		{
			if ($_SESSION["phonegap_os"] != "" && $_SESSION["phonegap_version"] != "") 
			{
				return;
			}
			else
			{
				trace("Phonegap flag without OS or version detected in session for {$_SERVER["REMOTE_ADDR"]} - resetting", 2);
				$_SESSION["phonegap"] = false;
			}
		}
		
		$matches = array();
		if (preg_match("/PhoneGap\\/Cordova (\\w+) (\\d+\\.\\d+\\.\\d+)/i", $_SERVER['HTTP_USER_AGENT'], $matches))
		{
			$_SESSION["phonegap"] = true;	
			$_SESSION["phonegap_os"] = $matches[1];
			$_SESSION["phonegap_version"] = $matches[2];

			trace("PhoneGap {$_SESSION["phonegap_version"]} for {$_SESSION["phonegap_os"]} enabled for {$_SERVER["REMOTE_ADDR"]} via User Agent", 2);
		}
		else if ($_POST["phonegap"] && $_POST["phonegap_version"] != "" && $_POST["phonegap_os"] != "")
		{
			$_SESSION["phonegap"] = true;
			$_SESSION["phonegap_version"] = $_POST["phonegap_version"];
			$_SESSION["phonegap_os"] = $_POST["phonegap_os"];
			
			trace("PhoneGap {$_SESSION["phonegap_version"]} for {$_SESSION["phonegap_os"]} enabled for {$_SERVER["REMOTE_ADDR"]} via POST", 2);
		}	
		else if (Settings::getValue("phonegap", "enable_phonegap_debugging") && $_GET["phonegap"])
		{
			$_SESSION["phonegap"] = true;
			$_SESSION["phonegap_version"] = Settings::getValue("phonegap", "phonegap_debug_version");
			$_SESSION["phonegap_os"] = Settings::getValue("phonegap", "phonegap_debug_os");
			
			trace("PhoneGap {$_SESSION["phonegap_version"]} for {$_SESSION["phonegap_os"]} enabled for {$_SERVER["REMOTE_ADDR"]} in debugging mode", 2);
		}
		
		if ($_SESSION["phonegap_os"] == "droid") $_SESSION["phonegap_os"] = "android";
	}
	
	static function hasPhonegap()
	{
		if (!Settings::getValue("phonegap", "enable_phonegap")) return false;
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
	
	/**
	 * Retrieve the current client device's OS (assuming it is running in Phonegap / Cordova)
	 * @return string the device OS
	 */
	static function getOS()
	{
		$os = $_SESSION["phonegap_os"];
		if ($os == "droid") $os = "android";
		return $os;
	}
	
	static function addScript($script)
	{
		global $phonegap;
		global $phonegap_csp;
		
		if (PhonegapManager::hasPhonegap())
		{
			ComponentManager::fireEvent("RegisterPhonegapFiles");
			ComponentManager::fireEvent("RegisterPhonegapAppScripts");
			ComponentManager::fireEvent("RegisterPhonegapSupportScripts");
			
			$os = PhonegapManager::getOS();
			
			//AJG: Do not add phonegap script references if we do not definitively know the OS
			if (!$os) return $script;
			
			trace(print_r(PhonegapManager::$appScripts, true), 3);
			
			$file = PhonegapManager::getPhonegapFile($os, $_SESSION["phonegap_version"]);
			$app  = PhonegapManager::getPhonegapAppScript($os, $_SESSION["phonegap_version"]);
			
			if ($app)
			{
				$phonegap = "<script type='text/javascript' src='{$app}'></script>\n".$phonegap;
			}

			if (Settings::getValue("phonegap", "use_local_phonegap_files"))
			{
				$phonegap_csp = "gap: cdvfile:";
				$phonegap = "<script type='text/javascript' src='cdvfile://localhost/bundle/www/cordova.js'></script>".$phonegap;
			}
			else
			{
				$phonegap_csp = "gap:";
				$phonegap = "<script type='text/javascript' src='{$file}'></script>\n".$phonegap;
			}		
			foreach(PhonegapManager::$supportScripts as $supportScript => $dummy)
			{
				$phonegap .= "<script type='text/javascript' src='{$supportScript}'></script>\n";
			}
		}
		else
		{
			$phonegap = "";
		}
		
		return $script;
	}
	
	static function registerPhonegapFile($platform, $version, $file)
	{
		trace("Registering Phonegap file for $platform $version: $file", 3);
		
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
	
	static function registerPhonegapAppScript($platform, $version, $file)
	{
		trace("Registering Phonegap App Script for $platform $version: $file", 3);
		PhonegapManager::$appScripts["{$platform}-{$version}"] = $file;
	}
	
	static function getPhonegapAppScript($platform, $version)
	{
		$key = "{$platform}-{$version}";
		if (array_key_exists($key, PhonegapManager::$appScripts))
		{
			return PhonegapManager::$appScripts[$key];
		}
		
		$key = "*-{$version}";
		if (array_key_exists($key, PhonegapManager::$appScripts))
		{
			return PhonegapManager::$appScripts[$key];
		}
		
		if (array_key_exists("*-*", PhonegapManager::$appScripts))
		{
			return PhonegapManager::$appScripts["*-*"];
		}
		
		return "";
	}
	
	static function registerPhonegapSupportScript($s)
	{
		PhonegapManager::$supportScripts[$s] = $s;
	}
	
	static function defaultSupportScripts()
	{
		PhonegapManager::registerPhonegapSupportScript("/components/phonegap/js/photo_uploader.js");
		PhonegapManager::registerPhonegapSupportScript("/components/phonegap/js/photo_attachment_uploader.js");	
	}
}

?>