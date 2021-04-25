<?php
/**\addtogroup admin
 * @{
 */

class AdminManager
{
	private static $proxyShims = null;
	
	static function registerAdminProxyShim($id, $shim)
	{
		if (!AdminManager::$proxyShims)
		{
			AdminManager::$proxyShims = array();
		}
		
		AdminManager::$proxyShims[$id] = $shim;
	}
	
	static function getAdminProxyShim()
	{
 		global $identifier;

 		if (!AdminManager::$proxyShims)
 		{
 			ComponentManager::fireEvent("RegisterAdminProxyShims");
 		}
 		
 		return AdminManager::$proxyShims[$identifier];
	}
	
	static function startFrame($class = "")
	{
		global $script;
		$script = Fakoli::getScripts() . $script;
		$styles = Fakoli::getStyles();		
		$css = explode(",", Settings::getValue("online_help", "help_css"));
		
		foreach($css as $c)
		{
			$styles .= "<link href='$c' rel='stylesheet'/>\n";
		}
?>
<!DOCTYPE html>
<html>
 <head>
 <?echo $styles?>
 <?echo $script?>
 </head>
 <body class='<?echo $class?>'>
<?
	}
	
	static function endFrame()
	{
?>
 </body>
</html>
<?
	}
}
/**@}*/?>