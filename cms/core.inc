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


register_shutdown_function(array(Fakoli, shutdownHandler));

require_once realpath(dirname(__FILE__)."/../framework/data_item.inc");
require_once realpath(dirname(__FILE__)."/../framework/cache.inc");
require_once realpath(dirname(__FILE__)."/components/component/datamodel/component.inc");

// Ensure that short open tags are enabled.
ini_set("short_open_tag", 1);

/**
 * FakoliException is the base exception class for all
 * Fakoli errors.
 * 
 * @author andy
 */
class FakoliException extends Exception {}

/**
 * FakoliEarlyExit is an internal exception class used to
 * provide for controlled early exits during page and handler 
 * execution.
 * 
 * @author andy
 */
class FakoliEarlyExit extends Exception {}


/**
 * FakoliEventNotConsumed is thrown when no subscriber has handled
 * an event that was fired with $mustBeConsumed set to true.
 */
class FakoliEventNotConsumed extends Exception {}

/**
 * This class provides the Fakoli CMS core. It contains basic utility functions
 * used by components and modules to access the component map, and provides facilities
 * for serving resources and pages to the client.
 * 
 * @author andy
 *
 */
class Fakoli
{
	static $componentsUsed = array();	///< Array of the components that have been referenced in this page request
	static $scanning = false;			///< Semaphore indicating that a component scan is in progress
	static $outputBufferLevel = 0;		///< Records the level at which gzip output buffering began
	static $rollbackBuffers = true;		///< Specifies whether to rollback output buffers when there is an unhandled exception. 
	static $storage = array();			///< General storage (volatile). See Fakoli::put and Fakoli::get
	static $timestamp;		  			///< Microsectond Timestamp for start of request
	static $requestStartTime; 			///< Start of request in DateTime format
	static $requestURI; 				///< The initiali request URI
	static $bodyClass = "";				///< Override for the body class of the page currently being served

	/**
	 * Stores a value or object at the given key. This storage is volatile and 
	 * limited to the scope of the current page request. This mechanism can be
	 * used to store information that needs to be communicated between modules
	 * rendering on the same page, or for other, similar purposes.
	 * @param string $key the key to store the data under
	 * @param unknown $obj the object or value to be stored.
	 */
	static function put($key, $obj) { Fakoli::$storage[$key] = $obj; }
	
	/**
	 * Retrieves the value or object at the given key. This storage is volatile and 
	 * limited to the scope of the current page request. This mechanism can be
	 * used to store information that needs to be communicated between modules
	 * rendering on the same page, or for other, similar purposes.
	 * @param string $key the key for the data to be retrieved
	 * @return multitype:
	 */
	static function get($key) { return Fakoli::$storage[$key]; }
	/**
	 * Import the datamodels, views and manifest for the specified component(s).
	 * More than one component can be specified in the call.
	 * 
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
				foreach($includes as $include)
				{
					require_once $include;
				}
				continue;
			}
	
			$components = Fakoli::getComponents();
			
			if (!$components[$component])
			{
				trace("Attempt to reference unknown or disabled component '$component'", 2);
				continue;
			}
			
			$includes = array();
			
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

			    		$includes[] = $f;
			    		require_once $f;
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

		    		$includes[] = $f;
		    		require_once $f;
	    		}
	    	}
	    	
	    	closedir($handle);
	    	
	    	Cache::put("fakoli_includes_$component", $includes);    	
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
			$component = new Component();
			if (!$component->tableExists()) Fakoli::bootstrap();
		
			$components = indexedQuery(Component, "WHERE enabled=1", "name");
			if (count($components) == 0)
			{
				Fakoli::initialComponentScan();
				$components = Cache::get("fakoli_components");
			}
			else
			{
				Cache::put("fakoli_components", $components);
			}
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
	
	/**
	 * Performs a complete component scan if the component table
	 * is found to be empty.
	 */
	static function initialComponentScan()
	{
		global $config;
		
		require_once realpath(dirname(__FILE__)."/components/component/datamodel/component.inc");
		require_once realpath(dirname(__FILE__)."/components/component/datamodel/component_page.inc");
		require_once realpath(dirname(__FILE__)."/components/component/datamodel/admin_page.inc");
		require_once realpath(dirname(__FILE__)."/components/component/datamodel/component_update_log.inc");
		require_once realpath(dirname(__FILE__)."/components/component/component_manager.inc");
		
		$mgr = new ComponentManager();
		$mgr->scan($config["scan_verbosity"]);
	}
	
	/**
	 * Bootstraps the creation of the database schema if the component
	 * table does not exist.
	 */
	static function bootstrap()
	{
		require_once realpath(dirname(__FILE__)."/components/component/datamodel/component.inc");
		require_once realpath(dirname(__FILE__)."/components/component/datamodel/component_page.inc");
		require_once realpath(dirname(__FILE__)."/components/component/datamodel/admin_page.inc");
		require_once realpath(dirname(__FILE__)."/components/component/datamodel/component_update_log.inc");
		require_once realpath(dirname(__FILE__)."/components/component/serialization_manager.inc");
		require_once realpath(dirname(__FILE__)."/components/component/component_manager.inc");
		require_once realpath(dirname(__FILE__)."/components/component/upgrade_manager.inc");
		require_once realpath(dirname(__FILE__)."/components/component/component_upgrade_manager.inc");
		
		$mgr = new ComponentUpgradeManager();
		$mgr->bootstrap();
	}	
		
	/**
	 * Assert that the user has one of the specified roles.
	 * @param string $role comma-separated list of acceptable roles
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
	
	/**
	 * Check that the user has one of the specified roles. If the $msg parameter is set and the
	 * user does not have the right role, that message is thrown in an exception.
	 * @param string $role comma-separated list of acceptable roles
	 * @param string $msg optional error message to be displayed on failure. If not specified, then false is returned on failure
	 * @throws FakoliException
	 * @return true if the user has one of the specified roles, false otherwise
	 */
	static function checkRole($role, $msg = null)
	{
		$access = checkRole($role);
		if (!$access && $msg) throw new FakoliException($msg);
		return $access;
	}
	
	/**
	 * Checks if the given user is active
	 * @param Object $user
	 */
	static function isUserActive($user)
	{
		$mgr = new UserManager();
		if (method_exists($mgr, "isActive")) return $mgr->isActive($user);
		if ($user->hasField("active")) return $user->active;
		return false;
	}
	
	/**
	 * Returns the local IP address for the server
	 */
	static function getLocalIP()
	{
		$ip = Settings::getValue("settings", "local_IP_address");
		return ($ip) ? $ip : $_SERVER["SERVER_ADDR"];
	}
	
	/**
	 * Uses the specified framework file(s) from the framework directory.
	 */
	static function usingFile()
	{
		$num = func_num_args();
		
		for($i = 0; $i < $num; ++$i)
		{
			$file = func_get_arg($i);
			
			$f = realpath(dirname(__FILE__)."/../$file");
			require_once $f;
		}
	}
	
	/**
	 * Uses the specified framework feature(s). Each feature corresponds to
	 * a file in the framework directory (without the extension). Use this 
	 * method to select the framework functionality you require for your page,
	 * (such as 'auto_form' or 'data_view').
	 */
	static function usingFeature()
	{
		$num = func_num_args();
		
		for($i = 0; $i < $num; ++$i)
		{
			$file = func_get_arg($i);
			
			$f = realpath(dirname(__FILE__)."/../framework/{$file}.inc");
			require_once $f;
		}
	}	

	/**
	 * Builds the administration menu, combining menu items supplied by
	 * the registered components in their manifests.
	 */
	static function getAdminMenu()
	{
		$menu = Cache::get("fakoli_admin_menu");
		if ($menu) return $menu;
		
		$menu = array("Site Content"			=> array(),
					  "Site Configuration"		=> array(),
					  "Collaboration"			=> array(),	
					  "Classification"			=> array(),
					  "Email Management"		=> array(),
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
	
	/**
	 * Returns the HTML scripts tags for Javascript files specified by
	 * the registered components in their manifest files.
	 */
	static function getScripts()
	{
		$scripts = Cache::get("fakoli_scripts");
		if ($scripts) return $scripts;
		
		$scriptArray = Fakoli::loadScripts();

		$scriptArray = ComponentManager::fireEvent("PreProcessScripts", $scriptArray);
		
		$scripts = "";
		foreach($scriptArray as $s)
		{
			$scripts .= "  <script type='text/javascript' src='$s'></script>\n";
		}
		
		Cache::put("fakoli_scripts", $scripts);
		return $scripts;
	}
	
	/**
	 * Load the list of JS scripts from the manifests of all registered, active components.
	 * @return array an array containing all the script paths in the correct include order.
	 */
	static function loadScripts()
	{
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
		
		return $scriptArray;
	}
	
	/**
	 * Returns the HTML link tags for CSS files specified by
	 * the registered components in their manifest files.
	 */
	static function getStyles()
	{
		$styles = Cache::get("fakoli_styles");
		if ($styles) return $styles;

		$styleArray = Fakoli::loadStyles();		
		$styleArray = ComponentManager::fireEvent("PreProcessStyles", $styleArray);
		
		$styles = "";
		foreach($styleArray as $s)
		{
			$styles .= "  <link href='$s' rel='stylesheet' type='text/css' media='all'/>\n";
		}
		
		Cache::put("fakoli_styles", $styles);
		return $styles;		
	}
	
	/**
	 * Load the list of CSS files from the manifests of all registered, active components.
	 * @return array an array containing all the file paths in the correct include order.
	 */
	static function loadStyles()
	{
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
		
		return $styleArray;
	}
	
	/**
	 * Utility function to pull in a specific Javascript file for the current page.
	 * @param string $src the URL of the Javascript file
	 */
	static function includeScript($src)
	{
		global $script;
		$script .= "<script type='text/javascript' src='$src'></script>\n";		
	}
	
	/**
	 * Utility function to pull in a specific CSS Stylesheet for the current page.
	 * @param string $src the URL of the stylesheet
	 * @param string $media the target media for the stylesheet (defaults to 'all')
	 */
	static function includeStylesheet($src, $media="all")
	{
		global $styles;
		$styles .= "<link href='$src' rel='stylesheet' type='text/css' media='$media'/>\n";
	}
	
	/**
	 * Sorting callback function that sorts two admin menu items by their specified weights.
	 * Used during the construction of the admin menu.
	 * @param array $item1
	 * @param array $item2
	 */
	static function compareByWeight($item1, $item2)
	{
		return $item1["weight"] - $item2["weight"];
	}
	
	/**
	 * Retrieves the capitalized camlCase name for the specified component.
	 * This is the root of the name for the component's manifest class. 
	 * @param string $name the component's name
	 */
	static function getComponentClassRoot($name)
	{
		return str_replace(" ", "", prettify($name));
	}
	
	/**
	 * Sends the resource at the specified URI to the client. This is used
	 * by the resource.php dispatcher to send files owned by the library that
	 * may not be within the web root.
	 * 
	 * @param unknown_type $uri the URI to the resource
	 * @param unknown_type $component the component that owns the resource
	 */
	static function sendResource($uri, $component = "")
	{
		$resource = Fakoli::resolveResource($uri, $component);
		if (!$resource)
		{
			throw new FakoliException("Cannot find resource '$uri'");
		}
		
		Fakoli::sendFile($resource);
	}
	
	/**
	 * Sends the contents of the specified file to the client.
	 * @param string $resource the full local file path of the resource
	 */
	static function sendFile($resource)
	{
	    $last_modified = filemtime($resource);
		
		$mimeType = DocumentHandler::getMIMEType($resource);
		
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

		header("Content-Type: $mimeType");	
		header("Last-Modified: ".date('r', $last_modified));
		header("Cache-Control: max-age=300, must-revalidate");
		header("Pragma: private");
		header("Expires: " . date(DATE_RFC822,strtotime(" 2 day")));
		
		if (!$noChange)
		{
			if (Fakoli::detectXSendFileSupport())
			{
				header("X-SendFile: $resource");
				return;	
			}
			else
			{
				$f = fopen($resource, "r");
		
			    $size = filesize($resource);
				header("Content-Length: $size");
						
				fpassthru($f);
				fclose($f);
			}
		}
		
		ComponentManager::fireEvent("RequestComplete");
	}
	
	static function detectXSendFileSupport()
	{
		if (function_exists("apache_get_modules"))
		{
			$support = in_array('mod_xsendfile', apache_get_modules());
		}
		
		return $support;
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
		
		if (endsWith($resource, ".inc") || endsWith($resource, ".php"))
		{
			header("HTTP/1.0 403 Forbidden");
			throw new FakoliException("Access Forbidden");			
		}
		
		$resource = sanitizePath($resource);
		
		$dir = dirname($resource);
		
		// Exclude the following component sub-directories
		if ($dir == "datamodel" || $dir == "updates" || $dir == "admin") return null;
		
		$components = Fakoli::getComponents();
		
		if ($component)
		{
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
		if (isset($config["extra_includes"])) $path .= PATH_SEPARATOR . $config["extra_includes"];
		
		$roots = explode(PATH_SEPARATOR, $path);
			
		foreach($roots as $root)
		{
			$file = sanitizePath($root . DIRECTORY_SEPARATOR . $resource);
			if (file_exists($file))
			{
				Cache::put("resource:$resource", $file);
				return $file;
			}
			
			$file = sanitizePath($root . DIRECTORY_SEPARATOR . "cms" . DIRECTORY_SEPARATOR . $resource);
			if (file_exists($file))
			{
				Cache::put("resource:$resource", $file);
				return $file;
			}			
		}

		foreach($components as $name => $c)
		{
			$file = sanitizePath($c->component_path . DIRECTORY_SEPARATOR . $resource);
			if (file_exists($file))
			{				
				Cache::put("resource:$resource", $file);
				return $file;
			}
		}
				
		trace("Cannot resolve $file", 2);
		
		return null;
	}
	
	/**
	 * Serves the page with the specified section and identifier.
	 * 
	 * @param string $sectionIdentifier the identifier of the section
	 * @param string $identifier the identifier of the page
	 * @param int $page_id numeric page identifier (deprecated)
	 */
	static function sendPage($sectionIdentifier, $identifier, $page_id = null)
	{
		trace("Send Page: '$sectionIdentifier', '$identifier', '$page_id'", 2);
		
		$site = Site::getSite();
		
		if (!$sectionIdentifier) $sectionIdentifier = "/";
		
		global $config;
		global $section;
		
		try
		{
			$section = Section::createFromIdentifier($sectionIdentifier);

			ComponentManager::fireEvent("OverrideResource", $identifier);
					
			if (!$page_id && !$identifier)
			{
				redirect($section->getDefaultPage());
			}
			
			if ($page_id)
			{
				$page = new Page($page_id);
			
				$page_role = $page->role;
				
				if (!checkRole($page->role))
				{
					LoginManager::redirectToLogin();	
				}
				
				echo $pageView->drawView();
				
			}
			else
			{
				if (!$section)
				{
					throw new FakoliException("The section you were attempting to access could not be found.");
				}
			
				$content = $section->getContent($identifier);
				if (!$content)
				{
					ComponentManager::fireEvent("ResourceNotFound", $identifier);
					
					throw new FakoliException("The page you were attempting to access could not be found.");
				}
				
				$permissions = $section->getPermissions($content);
				if ($permissions)
				{
					if (!Settings::checkPermissions($section->getPermissions($content)))
					{
						LoginManager::redirectToLogin();		
					}
				}
				else if (!checkRole($section->getRole($content)))
				{
					LoginManager::redirectToLogin();		
				}
				
				trace("\$_SERVER['HTTPS'] = {$_SERVER['HTTPS']}", 3);
				trace("\$_SERVER['REQUEST_URI'] = {$_SERVER['REQUEST_URI']}", 3);
				
				$https = $_SERVER['HTTPS'];
				if ($https == "off") $https = false; // For IIS
			
				$useSSL = $section->use_SSL || $content->use_SSL;
				
				if ($useSSL && !$https )
				{
					redirect("https://".$config['http_host'].$_SERVER['REQUEST_URI']);
				}
				else if (!$useSSL && $https && $identifier != "login" && !isset($_REQUEST["login"]))
				{
					redirect("http://".$config['http_host'].$_SERVER['REQUEST_URI']);
				}
			
				try
				{
					ComponentManager::fireEvent("ResolveIdentifier", $identifier, true);
				}
				catch(FakoliEventNotConsumed $e)
				{
					throw new FakoliException("The resource '$identifier' is disabled");
				}
			}
			
			ComponentManager::fireEvent("RequestComplete");		
		}
		catch(FakoliEarlyExit $exit)
		{
			echo $exit->getMessage();
		}		
	}
	
	/**
	 * Send the admin page for the given identifier
	 * @param string $identifier the admin page to be sent to the client
	 */
	static function sendAdminPage($identifier)
	{
		global $config;
		
		if ($identifier != "login" && $identifier != "password_help") 
		{
			Fakoli::assertRole("admin", "/admin/login");
		}
		
		$https = $_SERVER['HTTPS'];
		if ($https == "off") $https = false; // For IIS
		
		if ($config['admin_SSL'] && !$https )
		{
			redirect("https://".$config['http_host'].$_SERVER['REQUEST_URI']);
		}
		
		$view = new AdminPageView($identifier);
		$view->drawView();		
	}
	
	/**
	 * Find the component containing the specified class.
	 */
	static function getComponentForClass($class)
	{
		$reflector = new ReflectionClass($class);
		$file = $reflector->getFileName();
		trace("File for $class: $file", 3);
		
		if ($file)
		{
			$file = str_replace(DIRECTORY_SEPARATOR, "/", $file);
			
			$matches = array();
			if (preg_match("/components\\/(.*?)\\//", $file, $matches))
			{
				trace("Component for $class is {$matches[1]}", 3);				
				return $matches[1];
			}
		}
		
		throw new FakoliException("Failed to determine component for $class");
	}
	
	/**
	 * Use this method to terminate execution of a script instead
	 * of using the php keywords exit() or die().
	 * @param unknown_type $message
	 */
	static function end($message)
	{
		throw new FakoliEarlyExit($message);
	}
	
	/**
	 * Return a DataItem to the client in JSON format
	 * @param DataItem $object the object to be returned
	 */
	static function JSONreturn($object)
	{
		header("Content-Type: application/json");
		throw new FakoliEarlyExit('{"result": '.$object->toJSON().'}');
	}
	
	/**
	 * Return an error code to the client in JSON format, as an object
	 * with the single property 'error'.
	 * @param String $error the error message to be converted to an object and returned
	 */
	static function JSONerror($error)
	{
		header("Content-Type: application/json");
		
		$errorObject = (object)array('error' => $error);
		
		throw new FakoliEarlyExit(json_encode($errorObject));
	}
	 
	/**
	 * Utility method to display a notification message to the user when the
	 * page has finished loading. This is useful for displaying information after a form has
	 * been saved, etc.
	 * @param string $message the notification message to be displayed
	 * @param boolean $blocking (optional) true if the user has to dismiss the message manually, false (default) if it should disappear automatically
	 * @param string $button  (optional) button text to show for blocking notifications
	 */
	static function notify($message, $blocking = false, $button = "OK")
	{
		$message = jsSafe($message);
		
		if ($blocking)
		{
			$options = ", {blocking: true, buttonText: '$button'}";
		}
		global $script;
		$script .= 
<<<ENDSCRIPT
		<script type='text/javascript'>
		window.addEvent('load', function()
		{
			notification("$message"$options);
		});
		</script>
ENDSCRIPT;
	}
	
	/**
	 * Schedules a notification message to be displayed to the user at the next full page load.
	 * Use this when you need to display a notification after a redirect.
	 * @param string $message
	 */
	static function notifyOnNextPage($message, $blocking = false, $button = "OK")
	{
		$_SESSION["notification_pending"] = $message;
		$_SESSION["notification_pending_blocking"] = $blocking;
		$_SESSION["notification_pending_button"] = $button;
	}
	
	/**
	 * Display any previously scheduled notification message.
	 */
	static function showPendingNotification()
	{
		global $isAction;
		global $isResource;
		
		if (!$isAction &&  !$isResource && isset($_SESSION["notification_pending"]))
		{
			Fakoli::notify($_SESSION["notification_pending"], $_SESSION["notification_pending_block"], $_SESSION["notification_pending_button"]);
			unset($_SESSION["notification_pending"]);
			unset($_SESSION["notification_pending_blocking"]);
			unset($_SESSION["notification_pending_button"]);
		}
	}
	
	/**
	 * Set the trace log level. 
	 */
	static function setTraceLevel($level)
	{
		global $config;
		$config["trace_level"] = $level;		
	}
	
	/**
	 * Change to the configured trace level for application code
	 */
	static function applicationTraceLevel()
	{
		Fakoli::setTraceLevel(Settings::getValue("debug", "application_trace_level"));
	}
	
	/**
	 * Change to the configured trace level for Fakoli core code
	 */
	static function coreTraceLevel()
	{
		Fakoli::setTraceLevel(Settings::getValue("debug", "trace_level"));
	}
	
	/**
	 * Changed to the configured trace level for scheduled task worker code
	 */
	static function scheduledTaskTraceLevel()
	{
		Fakoli::setTraceLevel(Settings::getValue("debug", "scheduled_task_trace_level"));
	}
	
	/**
	 * Customer Error Handler
	 * @param unknown $errLevel
	 * @param unknown $errMsg
	 * @param unknown $errFile
	 * @param unknown $errLine
	 * @throws DOMException
	 * @throws ErrorException
	 */
	static function errorHandler($errLevel, $errMsg, $errFile, $errLine)
	{
		// Transform XML DOM warning messages into exceptions for easier handling
	    if ($errLevel==E_WARNING && (substr_count($errMsg,"DOMDocument::loadXML()")>0))
	    {
	        throw new DOMException($errMsg);
	    }
	    
		if ($errLevel & (E_NOTICE | E_WARNING | E_USER_WARNING | E_STRICT | E_DEPRECATED)) return;
		
		throw new ErrorException($errMsg, $errLevel, $errLevel, $errFile, $errLine);
	}

	/**
	 * Custom Exception Handler
	 * @param unknown $exception
	 * @return void|boolean
	 */
	static function exceptionHandler($exception)
	{
		global $isAction;
		
		try 
		{
	    	//Getting exception information
			$errMsg = $exception->getMessage();
			$errLevel = $exception->getCode();
			$errFile = $exception->getFile();
			$errLine = $exception->getLine();
	        
			if ($errLevel & (E_NOTICE | E_WARNING | E_USER_WARNING | E_STRICT | E_DEPRECATED)) return;
		
			trace("ZIP: Winding back output buffers from ".ob_get_level(), 3);
			
			if (Fakoli::$rollbackBuffers)
			{
				while(ob_get_level() > Fakoli::$outputBufferLevel); // Clear all output buffers.
				{
					ob_end_clean();
				}
			}
					
			$msg = "<br/><b>Stack trace:</b><br/>".$exception->getTraceAsString();
			
			if ($isAction)
			{
				echo $exception->getMessage().$msg;
			}
			else
			{
				$err = Fakoli::resolveResource("/fakoli/error.tpl");
				$template = file_get_contents($err);
				$out = str_replace("{message}", $msg, $template);
				echo $out;
			}
				
			ComponentManager::fireEvent("RequestComplete");
		}
		catch (Exception $ex)
		{
			echo $ex->getMessage();
			//Do nothing. 
			//Avoiding error from error handling...
		}
	    
		/* Don't execute PHP internal error handler */
	    return true;
	}
	
	/**
	 * Returns the name of the currently executing script
	 */
	static function scriptName()
	{
		list ($s, $qs) = explode("?", $_SERVER['REQUEST_URI']);
		return $s;		
	}
	
	/**
	 * Retrieves information about the Server CPU load. For Windows systems
	 * this is reported as percentage CPU. For Linux/UNIX systems the load 
	 * information is returned in uptime format.
	 * 
	 * Note that in order for the system to return correct load values on 
	 * Windows, the application pool must be running as a user with permission
	 * to read performance counters (such as NetworkService).
	 */
	static function getServerLoad()
	{
		$out = "";
		
		if (stristr(PHP_OS, 'win')) 
		{			
			if (class_exists("COM"))
			{
	            $wmi = new COM("Winmgmts:{impersonationlevel=impersonate}!\\Root\\Cimv2");
			  	$cpus = $wmi->InstancesOf("Win32_Processor");
	  
			  	foreach ($cpus as $cpu) 
			  	{
			   		$out .=  $cpu->LoadPercentage."% ";
			  	}
			}
			else
			{
				$out .= "Not Available";
			}			
        } 
        else 
        {       
            $sys_load = sys_getloadavg();
            $out = "$sys_load[0]&nbsp;&nbsp;&nbsp;$sys_load[1]&nbsp;&nbsp;&nbsp;$sys_load[2]";       
        }
       
        return $out;  
    }
    
    /**
     * Returns formatted summary information regarding usage of the server's filesystem.
     * @return string 
     */
    static function getFileSystemInfo()
    {
    	global $config;
    	
    	if (isset($config['filesystems']))
    	{
    		$fs = explode(",", $config['filesystems']);
    	}
    	else if (stristr(PHP_OS, "win"))
    	{
    		$fs = array("C:");
    	}
    	else
    	{
    		$fs = array("/");
    	}
    	
    	$out = "<table class='list'><thead><tr><th>File System</th><th>Size</th><th>Available</th><th>% Free</th></tr></thead><tbody>";
    	foreach($fs as $f)
    	{
    		$free = disk_free_space($f);
    		$total = disk_total_space($f);
    		$percent = number_format($free * 100 / $total, 1);
    		$style = ($percent < 10) ? " style='background-color: #f00;color: #fff'" : (($percent < 25) ? " style='background-color: #ff0'" : "");
    		$out .= "<tr$style><td>$f</td><td style='text-align: right'>" . getScaledSize($total) . 
    				"</td><td style='text-align: right'>" . getScaledSize($free) . 
    				"</td><td style='text-align: right'>" . $percent . "%</td></tr>";
    	}
    	
    	$out .= "</tbody></table>";
    	return $out;
    }
    
    /**
     * Custom shutdown handler
     */
    static function shutdownHandler()
    {
    	global $config;
    	$error = error_get_last();
    	
    	trace("## Shutdown", 3);
    	
	    if ($error)
	    {
	    	switch($error['type'])
	    	{
	        case E_ERROR:
	        case E_CORE_ERROR:
	        case E_COMPILE_ERROR:
	        case E_USER_ERROR:
			
		    	trace($error['message']." at ".$error['file'].":".$error['line'], 1);

	    	 	if ($config["dump_includes"]) 
	            {
	            	printIncludes();
	            }
	            break;
	        }
	    }   
    }
}

/**
 * Pull out a simple reference to the request method
 */
$method = $_SERVER["REQUEST_METHOD"];

Fakoli::$timestamp = microtime(true);
Fakoli::$requestStartTime = now();
Fakoli::$requestURI = $_SERVER["REQUEST_URI"];

$isAction = (basename($_SERVER["SCRIPT_FILENAME"]) == "action.php" || php_sapi_name == "cli");
$isResource = basename($_SERVER["SCRIPT_FILENAME"]) == "resource.php";

if ($isAction)
{
	registerURLRewriteParameters("component", "identifier");
}
else if ($isResource)
{
	registerURLRewriteParameters("component", "path");
}
else if ($isAdmin)
{
	registerURLRewriteParameters("identifier");
}
else
{
	registerURLRewriteParameters("section", "identifier");
}

Fakoli::$outputBufferLevel = ob_get_level(); // Initial level of output buffer nesting

set_error_handler(array("Fakoli", "errorHandler"));
set_exception_handler(array("Fakoli", "exceptionHandler"));

Fakoli::using("component", "user", "login");

ComponentManager::fireEvent("ConfigurationBootstrap");


// Automatically import commonly used features
Fakoli::usingFeature("auto_form", "auto_form_layout", "data_view", "grouped_data_view", "join", "search_form", "tab_bar", "paged_list", "expanding_list");

$user = null;

// Import Standard Features
Fakoli::usingFeature("data_item", "auto_form", "field_renderers", "cache", "data_view", "grouped_data_view", "filter_form", "search_form", 
					 "paged_list", "grouped_list", "facet_manager", "join", "tab_bar", "tree");

$config["trace_level"] = Settings::getValue("debug", "trace_level");
					
ComponentManager::fireEvent("RegisterSessionPersistance");
ComponentManager::fireEvent("RegisterLoginExtension");

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

Fakoli::showPendingNotification();
ComponentManager::fireEvent("Initialize");

?>