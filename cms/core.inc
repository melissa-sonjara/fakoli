<?php
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

require_once realpath(dirname(__FILE__)."/../framework/data_item.inc");
require_once realpath(dirname(__FILE__)."/../framework/cache.inc");
require_once realpath(dirname(__FILE__)."/components/component/datamodel/component.inc");

class FakoliException extends Exception {}

/**
 * This class provides the Fakoli CMS core. It contains basic utility functions
 * used by components and modules to access the component map.
 * 
 * @author andy
 *
 */
class Fakoli
{
	static $componentsUsed = array();
	
	/**
	 * Import the datamodels, views and manifest for the specified component.
	 * @param string $component the component to import
	 */
	static function using()
	{
		$num = func_num_args();
		for($i = 0; $i < $num; ++$i)
		{
			$component = func_get_arg($i);
			
			if (array_key_exists($component, Fakoli::$componentsUsed)) continue;
	    	
	    	Fakoli::$componentsUsed[$component] = true;
			
			$includes = Cache::get("fakoli_includes_$component");
			if ($includes)
			{
				eval($includes);
				continue;
			}
	
			$components = Fakoli::getComponents();
			
			if (!$components[$component])
			{
				throw new FakoliException("Attempt to reference unknown or disabled component '$component'");
			}
			
			$c = $components[$component];
			
			$dm = $c->component_path . DIRECTORY_SEPARATOR . "datamodel";
			if (is_dir($dm))
			{
				$handle = opendir($dm);
				while(false !== ($file = readdir($handle))) 
		    	{
		    		if (endsWith($file, ".inc"))
		    		{
			    		$f = $dm . DIRECTORY_SEPARATOR . $file;
			    		
			    		$includes .= "require_once '{$f}';\n";
		    		}
		    	}
		    	
		    	closedir($handle);
			}
	
			trace($c->component_path, 4);
			
			$handle = opendir($c->component_path);
	
			if (!$handle) continue;
			
			while(false !== ($file = readdir($handle))) 
	    	{
	    		if (endsWith($file, ".inc"))
	    		{
		    		$f = $c->component_path . DIRECTORY_SEPARATOR . $file;
		    		
		    		$includes .= "require_once '{$f}';\n";
	    		}
	    	}
	    	
	    	closedir($handle);
	    	
	    	Cache::put("fakoli_includes_$component", $includes);
	    	eval($includes);    	
		}
	}
	
	/**
	 * Retrieve an array of all the currently enabled components, indexed by component name.
	 * @return array
	 */
	static function getComponents()
	{
		$components = Cache::get("fakoli_components");
		
		if (!$components)
		{
			$components = indexedQuery(Component, "WHERE enabled=1", "name");
			if (count($components) == 0)
			{
				Fakoli::initialComponentScan();
				$components = indexedQuery(Component, "WHERE enabled=1", "name");
			}
			
			Cache::put("fakoli_components", $components);
		}
		
		return $components;
	}
	
	/**
	 * Return the component record by name
	 * @param string $name the name of the component
	 * @return Component the component record for the specified component.
	 */
	static function getComponent($name)
	{
		$components = Fakoli::getComponents();
		return $components[$name];
	}
	
	/**
	 * Test whether the specified component has been registered
	 * @param string $name the name of the component to test
	 * @return boolean true if registered, false otherwise
	 */
	static function componentExists($name)
	{
		return array_key_exists($name, Fakoli::getComponents());
	}
	
	static function initialComponentScan()
	{
		require_once "cms/components/component/datamodel/component.inc";
		require_once "cms/components/component/datamodel/component_page.inc";
		require_once "cms/components/component/datamodel/admin_page.inc";
		require_once "cms/components/component/component_manager.inc";
		$mgr = new ComponentManager();
		$mgr->scan();
	}
	
	/**
	 * Assert that the user has one of the specified roles.
	 * @param string $role
	 * @param string $redirect the page to which the user is to be redirected on failure
	 */
	static function assertRole($role, $redirect = "/index")
	{
		global $user;
		
		if (!checkRole($role))
		{
			if (!$user)
			{
				$mgr = new LoginManager();
				$mgr->storeRedirectPage();
			}

			redirect($redirect);
		}
	}
	
	static function usingFile()
	{
		$num = func_num_args();
		
		for($i = 0; $i < $num; ++$i)
		{
			$file = func_get_arg($i);
			
			require_once realpath(dirname(__FILE__)."/../$file");
		}
	}
	
	static function usingFeature()
	{
		$num = func_num_args();
		
		for($i = 0; $i < $num; ++$i)
		{
			$file = func_get_arg($i);
			
			require_once realpath(dirname(__FILE__)."/../framework/{$file}.inc");
		}
	}	

	static function getAdminMenu()
	{
		$menu = Cache::get("fakoli_admin_menu");
		if ($menu) return $menu;
		
		$menu = array("Web Site Content"		=> array(),
					  "Web Site Configuration"	=> array(),
					  "Collaboration"			=> array(),	
					  "Classification"			=> array(),
					  "User Management"			=> array());		
		
		$components = Fakoli::getComponents();
		foreach($components as $name => $component)
		{
			$manifest = $component->loadManifest();
			if (method_exists($manifest, getAdminMenu))
			{
				$componentMenuItems = $manifest->getAdminMenu();
				if ($componentMenuItems)
				{
					foreach($componentMenuItems as $category => $items)
					{
						if (!$menu[$category])
						{
							$menu[$category] = $items;
						}
						else
						{	
							$menu[$category] = array_merge($menu[$category], $items);
						}
					}			
				}
			}
		}
		
		foreach($menu as $category => $items)
		{
			uasort($menu[$category], array(Fakoli, compareByWeight));
		}
		
		Cache::put("fakoli_admin_menu", $menu);
		
		return $menu;
	}
	
	static function getScripts()
	{
		$scripts = Cache::get("fakoli_scripts");
		if ($scripts) return $scripts;
		
		$scriptArray = array();
		
		$components = Query::create(Component, "WHERE enabled=1 ORDER BY priority")->execute();
		foreach($components as $component)
		{
			$manifest = $component->loadManifest();		
			if (method_exists($manifest, getScripts))
			{
				// Calculate union of scripts
				$scriptArray = array_merge($scriptArray, array_diff($manifest->getScripts(), $scriptArray));  
			}
		}

		$scriptArray = ComponentManager::fireEvent("PreProcessScripts", $scriptArray);
		
		$scripts = "";
		foreach($scriptArray as $s)
		{
			$scripts .= "  <script type='text/javascript' src='$s'></script>\n";
		}
		
		Cache::put("fakoli_scripts", $scripts);
		return $scripts;
	}
	
	static function getStyles()
	{
		$styles = Cache::get("fakoli_styles");
		if ($styles) return $styles;
		
		$styleArray = array();
		
		$components = Query::create(Component, "WHERE enabled=1 ORDER BY priority")->execute();
		foreach($components as $component)
		{
			$manifest = $component->loadManifest();		
			if (method_exists($manifest, getStyles))
			{
				// Calculate union of styles
				$styleArray = array_merge($styleArray, array_diff($manifest->getStyles(), $styleArray));  
			}
		}

		$styleArray = ComponentManager::fireEvent("PreProcessStyles", $styleArray);
		
		$styles = "";
		foreach($styleArray as $s)
		{
			$styles .= "  <link href='$s' rel='stylesheet' type='text/css' media='all'/>\n";
		}
		
		Cache::put("fakoli_styles", $styles);
		return $styles;		
	}
	
	static function compareByWeight($item1, $item2)
	{
		return $item1["weight"] - $item2["weight"];
	}
	
	static function getComponentClassRoot($name)
	{
		return str_replace(" ", "", prettify($name));
	}
	
	static function sendResource($uri, $component = "")
	{
		$resource = Fakoli::resolveResource($uri, $component);
		if (!$resource)
		{
			throw new FakoliException("Cannot find resource '$uri'");
		}
		
	    $last_modified = filemtime($resource);

		
		$mimeType = getMIMEType($resource);
		
	    if(array_key_exists("HTTP_IF_MODIFIED_SINCE",$_SERVER))
	    {
	    	
	        $if_modified_since=strtotime($_SERVER["HTTP_IF_MODIFIED_SINCE"]);
	    	//$if_modified_since=strtotime(preg_replace('/;.*$/','',$_SERVER["HTTP_IF_MODIFIED_SINCE"]));
	        
	        if($if_modified_since >= $last_modified)
	        {
	            header("HTTP/1.0 304 Not Modified");
	            $noChange = true;
	        }
	    }

		header("Last-Modified: ".date('r', $last_modified));
		header("Cache-Control: max-age=300, must-revalidate");
		header("Content-Type: $mimeType");
		header("Pragma: private");
		header("Expires: " . date(DATE_RFC822,strtotime(" 2 day")));
	    
		if (!$noChange)
		{
		    $f = fopen($resource, "r");
	
		    $size = filesize($resource);
			header("Content-Length: $size");
					
			fpassthru($f);
			fclose($f);
		}
	}
	
	/**
	 * Resolves the path to a  web resource based on the PHP include path.
	 * @param string $path the web resource path (URI)
	 * @param string $component (optional) the component that owns the resource
	 * @return string the full server path of the matching file.
	 */
	static function resolveResource($resource, $component = "")
	{
		global $config;
		
		if (strpos("../", $resource) !== FALSE)
		{
			throw new FakoliException("Attempt to resolve illegal resource path");
		}
		
		$resource = sanitizePath($resource);
		
		if ($component)
		{
			$components = Fakoli::getComponents();
			
			if (!$components[$component])
			{
				throw new FakoliException("Attempt to reference unknown or disabled component '$component'");
			}
			
			$c = $components[$component];
			
			$file = $c->component_path . DIRECTORY_SEPARATOR . $resource;
			
			return (file_exists($file)) ? $file : null;
		}
		
		$file = Cache::get("resource:$resource");
		if ($file) return $file;
		
		$path = $config['homedir'] . PATH_SEPARATOR . ini_get("include_path");
			
		$roots = explode(PATH_SEPARATOR, $path);
			
		foreach($roots as $root)
		{
			$file = $root . DIRECTORY_SEPARATOR . $resource;
			if (file_exists($file))
			{
				Cache::put("resource:$resource", $file);
				return $file;
			}
		}
				
		return null;
	}
}

Fakoli::using("user", "login", "component");

$user = null;

ComponentManager::fireEvent("RegisterSessionPersistance");

session_start();

if (isset($_SESSION["user"]))
{
	if ($_POST["Logout"])
	{
		session_destroy();
		unset($user);
	}
	else
	{
		$user = $_SESSION["user"];
	}
}
else
{

	if ($method == "POST" && $_POST["Login"])
	{	
		$mgr = new LoginManager();
		$mgr->validateLogin();
		
	}
}

ComponentManager::fireEvent("Initialize");

?>