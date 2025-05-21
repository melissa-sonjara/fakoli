<?php
/**\addtogroup redirect
 * @{
 */


/**
 *
 * Provides a central management class for event handlers and 
 * common functionality for the redirect component.
 */

class RedirectManager
{
	function __construct()
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
			$redirect = Query::create('Redirect', "WHERE redirect_from=:p")
							 ->bind(":p", $path)
							 ->executeSingle();
			
			if ($redirect->preserve_query_string)
			{
				$redirect = (strpos($redirect->redirect_to, "?") === false) ?
								$redirect->redirect_to."?".getCleanQueryString() : 
								$redirect->redirect_to."&".getCleanQueryString();
				redirect($redirect);
			}
			else
			{
				redirect($redirect->redirect_to);
			}
		}
		catch(DataNotFoundException $e)
		{
				
		}
	
		return $identifier;
	}
		
	static function handleSectionRedirects()
	{
		$redirects = Query::create('Redirect', "WHERE active=1 AND redirect_from=:p")
						 ->bind(":p", Fakoli::$requestURI)
						 ->execute();
		if (count($redirects) > 0)
		{
			redirect($redirects[0]->redirect_to);
		}
	}
	
	static function handleOverrides($identifier)
	{
		global $section;
		$path = "";
		if ($section->section != "/") $path .= "/" . $section->section;
		$path .= "/" . $identifier;
	
		trace("Redirect: $path", 3);
	
		$overrides = Cache::get("resource_overrides");
		
		if (!$overrides)
		{
			$overrides = IndexedQuery::create('Redirect', "WHERE override=1 and active=1", "redirect_from")->execute();
			Cache::put("resource_overrides", $overrides);
		}
		
		if (array_key_exists($path, $overrides))
		{
			$redirect = $overrides[$path];
			
			if ($redirect->exempt_roles && checkRole($redirect->exempt_roles)) return;
			if ($redirect->exemption_flag && $_SESSION[$redirect->exemption_flag]) return;
			
			if ($redirect->preserve_query_string)
			{
				$redirect = (strpos($redirect->redirect_to, "?") === false) ?
				$redirect->redirect_to."?".getCleanQueryString() :
				$redirect->redirect_to."&".getCleanQueryString();
				redirect($redirect);
			}
			else
			{
				redirect($redirect->redirect_to);
			}
		}
	}
	

	static function registerSerializationHandler()
	{
		SerializationManager::registerHandler("redirect", "Redirect Rules", new SimpleSerializationHandler(Redirect));
		return true;
	}
}

/**@}*/?>
