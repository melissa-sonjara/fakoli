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
		//TODO: Set default configuration parameters here
	}

	static function detectPhonegap()
	{
		trace(1, "Detecting PhoneGap");
		
		if ($_POST["phonegap"])
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