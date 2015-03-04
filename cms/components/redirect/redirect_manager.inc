<?php
/**
 *
 * Provides a central management class for event handlers and 
 * common functionality for the redirect component.
 */

class RedirectManager
{
	function RedirectManager()
	{

	}

	static function setDefaults()
	{
		//TODO: Set default configuration parameters here
	}



	static function upgradeComponent($version)
	{
		$mgr = new RedirectUpgradeManager();
		$mgr->upgrade($version);
	}
	
	static function handleRedirects($identifier)
	{
		global $section;
		$path = "";
		if ($section->section != "/") $path .= "/" . $section->section;
		$path .= "/" . $identifier;
		
		trace("Redirect: $path", 3);
		
		try
		{
			$redirect = Query::create(Redirect, "WHERE redirect_from=:p")
							 ->bind(":p", $path)
							 ->executeSingle();
			
			redirect($redirect->redirect_to);
		}
		catch(DataNotFoundException $e)
		{
				
		}
	
		return $identifier;
	}
		
}

?>