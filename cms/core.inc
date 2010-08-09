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
 * This class provides the Fakoli CMS core. It contains almost no functionality
 * of its own - everything is defined as components.
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
	
			trace($c->component_path, 3);
			
			$handle = opendir($c->component_path);
	
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
			Cache::put("fakoli_components", $components);
		}
		
		return $components;
	}
	
	/**
	 * Assert that the user has one of the specified roles.
	 * @param string $role
	 * @param string $redirect the page to which the user is to be redirected on failure
	 */
	static function assertRole($role, $redirect = "/index")
	{
		if (!checkRole($role)) redirect($redirect);
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
	
	static function getAdminMenu()
	{
		$menu = Cache::get("fakoli_admin_menu");
		if ($menu) return $menu;
		
		$menu = array("Web Site Content"	=> array(),
					  "Collaboration"		=> array(),	
					  "Classification"		=> array(),
					  "User Management"		=> array());		
		
		$components = Fakoli::getComponents();
		foreach($components as $name => $component)
		{
			$manifest = $component->loadManifest();

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
		
		foreach($menu as $category => $items)
		{
			uasort($menu[$category], array(Fakoli, compareByWeight));
		}
		
		Cache::put("fakoli_admin_menu", $menu);
		
		return $menu;
	}
	
	static function compareByWeight($item1, $item2)
	{
		return $item1["weight"] - $item2["weight"];
	}
	
	static function getComponentClassRoot($name)
	{
		return str_replace(" ", "", prettify($name));
	}
}

Fakoli::using("user");

$user = null;

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

	if ($method == "POST")
	{
		if ($_POST["Login"])
		{	
			$user = querySingle(SiteUser, "WHERE username='{$_POST["username"]}' and active=1");
			
			if ($user && crypt($_POST["password"], $user->password) === $user->password)
			{
				session_start();
				
				$_SESSION["user"] = $user;
			}
			else
			{
				$login_error = "<span style='color: red'>Incorrect user name or password</span><br/>";
			}
		}
	}
}
?>