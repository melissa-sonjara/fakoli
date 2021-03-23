<?php
/**\addtogroup site
 * @{
 */


/**************************************************************

 Copyright (c) 2010 Sonjara, Inc

 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:

 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.

 Except as contained in this notice, the name(s) of the above 
 copyright holders shall not be used in advertising or otherwise 
 to promote the sale, use or other dealings in this Software 
 without prior written authorization.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.

*****************************************************************/

Fakoli::using("site");

class SiteManager
{
	static $themes = null;
	
	function SiteManager()
	{
		
	}
	
	static function registerTheme($component, $name, $description)
	{
		$component = ComponentManager::getComponent($component);
		
		SiteManager::$themes[$component->name] = array("name" => $name, 
													   "description" => $description, 
												       "directory" => $component->component_path);
	}
	
	static function registerThemes()
	{
		SiteManager::$themes = array();
		ComponentManager::fireEvent("RegisterTheme");
		Cache::put("registered_themes", SiteManager::$themes);
		return SiteManager::$themes;
	}
	
	static function getThemes()
	{
		$themes = Cache::get("registered_themes");
		
		if (!$themes)
		{
			$themes = SiteManager::registerThemes();
		}
		return $themes;
	}
	
	static function getThemeDirectory($theme = "")
	{
		global $config;
		
		if ($theme == "")
		{
			return $config['homedir'] . DIRECTORY_SEPARATOR . "templates";
		}	
		
		$themes = SiteManager::getThemes();
		
		if (!array_key_exists($theme, $themes))
		{
			trace("Unknown theme '$theme' - using default", 2);
			return $config['homedir'] . DIRECTORY_SEPARATOR . "templates";
		}
		return $themes[$theme]["directory"];
	}
	
	static function getThemeOptions()
	{
		$themes = SiteManager::getThemes();
		
		$options = array("" => "Default");
		foreach($themes as $component => $theme)
		{
			$options[$component] = $theme["name"];
		}
		
		return $options;
	}
	
	static function formatThemeName($site)
	{
		if (!$site->theme) return "Default";
		$themes = SiteManager::getThemes();
		
		if (!array_key_exists($site->theme, $themes))
		{
			return "ERROR";
		}
		
		return $themes[$site->theme]["name"];
	}		
	
	static function upgradeComponent($version)
	{
		$mgr = new SiteUpgradeManager();
		$mgr->upgrade($version);
	}	
}
/**@}*/?>