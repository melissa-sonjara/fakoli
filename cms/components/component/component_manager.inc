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

/** \addtogroup component
 * @{
 */

/**
 * ComponentManager provides the core functionality for building the 
 * component map describing the application configuration. It also
 * scans components to build the administration section page map, 
 * and provides an event dispatching mechanism allowing PHP or client-side
 * scripts to pass events or perform actions on a component without
 * directly instantiating any classes related to the component.
 *
 * @author andy
 *
 */
class ComponentManager
{
	var $found = array();
	
	static $verbosityLevel = 0;		///< Verbosity level for component scan (same as trace level)
	static $plainOutput = false;	///< Set to true for plain text output during component scan
	private static $scanLog = "";
	
	static function scanTrace($message, $level)
	{
		global $config;
		
		if ($level <= ComponentManager::$verbosityLevel)
		{
			if (!ComponentManager::$scanLog)
			{
				ComponentManager::$scanLog = $config['uploadbase'].DIRECTORY_SEPARATOR."component_scan_".date("Ymd-His").".log";
			}
			
			error_log($message."\n", 3, ComponentManager::$scanLog);
			
			echo $message;
			echo (ComponentManager::$plainOutput) ? "\n" : "<br/>";
		}
		
		trace($message, $level);
	}
	
	/**
	 * Creates a new ComponentManager object
	 */
	function __construct()
	{
	}
	
	/**
	 * Scan the specified root and sub-directory for components.
	 *
	 * @param $root
	 * @param $subdir
	 */
	function scanComponents($root, $subdir)
	{
		if (!$root || $root == ".") return;
		
		$path = $root . DIRECTORY_SEPARATOR . str_replace("/", DIRECTORY_SEPARATOR, $subdir);
	
		ComponentManager::scanTrace("Scanning $path", 2);
		
		if (is_dir($path))
		{
			$handle = opendir($path);
			while(false !== ($file = readdir($handle))) 
		    {
		    	$f = $path . DIRECTORY_SEPARATOR . $file;
		    	$m = $f . DIRECTORY_SEPARATOR . "manifest.inc";
		    	$d = $f . DIRECTORY_SEPARATOR . "DEPRECATED";
		    	
		    	if (is_dir($f))
		    	{
		    		ComponentManager::scanTrace("== Found component '$file'", 3);
		    		if (is_file($d))
		    		{
		    			ComponentManager::scanTrace("**** Deprecated component - ignoring", 3);
		    			Cache::invalidate("fakoli_includes_$file");
		    			continue;
		    		}
		    		
		    		if ($this->found[$file])
		    		{
		    			ComponentManager::scanTrace("---- already defined - skipping", 3);
		    			continue;
		    		}
		    		
		    		if (!is_file($m))
		    		{
		    			ComponentManager::scanTrace("== No manifest found. Skipping...", 3);
		    			continue;
		    		}
		    		
		    		require_once $m;
		    		
		    		$cl = Fakoli::getComponentClassRoot($file) . "Manifest";
		    		
		    		try
		    		{
			    		ComponentManager::scanTrace("== Instantiating $cl", 3);
			    		
			    		$manifest = new $cl;
			    		$component = $manifest->getComponentDefinition();
			    		$component->component_path = sanitizePath($f);
			    		$component->joinTransaction($this->tx);
			    		$component->save();
			    		
			    		$this->found[$file] = $component;
			    		
			    		$this->scanAdminPages($component);
			    		$this->scanComponentPages($component);
		    		
		    		}
		    		catch(Exception $e)
		    		{
		    			ComponentManager::scanTrace("-- Failed to instantiate manifest class. Skipping...", 3);	    		
		    		}
	
		    		Cache::invalidate("fakoli_includes_$file");
		    	}
		    }
		    
		    closedir($handle);
		}
	}

	/**
	 * Builds the administration page map for the specified component.
	 * @param $component the component being scanned
	 */
	function scanAdminPages($component)
	{
		if (!$component->enabled) return;
		
		$admin = $component->component_path . DIRECTORY_SEPARATOR . "admin";
		
		if (!is_dir($admin)) return;
		
		ComponentManager::scanTrace("== Scanning $admin", 3);
		
		$handle = opendir($admin);
		
		while(false !== ($file = readdir($handle))) 
		{
			if (endsWith($file, ".inc"))
			{
				$page = new AdminPage();
				$page->identifier = preg_replace("/\\.inc$/", "", $file);
				$page->server_path = $admin . DIRECTORY_SEPARATOR . $file;
				$page->component_id = $component->component_id;
				$page->joinTransaction($this->tx);
				$page->save();
				Cache::invalidate("fakoli_admin_pages_{$page->identifier}");				
			}
		}
		
	}
		
	/**
	 * Scan and build the compoment page map for the specified component.
	 * @param $component the component to be scanned.
	 */
	function scanComponentPages($component)
	{
		$pages = $component->component_path . DIRECTORY_SEPARATOR . "pages";
		
		if (!is_dir($pages)) return;
		ComponentManager::scanTrace("== Scanning $pages", 3);
		
		$handle = opendir($pages);
		
		while(false !== ($file = readdir($handle))) 
		{
			if (endsWith($file, ".inc"))
			{
				$identifier = preg_replace("/\\.inc$/", "", $file);
				
				try
				{
					$page = Query::create('ComponentPage', "WHERE component=:c AND identifier=:i")
							->bind(":c", $component->name, ":i", $identifier)
							->executeSingle();
							
					$page->scanned = true;
					$page->server_path = $pages . DIRECTORY_SEPARATOR . $file;
					$page->filter = new InclusionFilter("component_page_id", "server_path", "scanned");
					$page->save();
				}
				catch(DataNotFoundException $e)
				{
					$page = new ComponentPage();
					$page->page_title = prettify($identifier);
					$page->identifier = $identifier;
					$page->component = $component->name;
					$page->server_path = $pages . DIRECTORY_SEPARATOR . $file;
					$page->role = "";
					$page->enabled = true;
					$page->scanned = true;
					$page->save();
				}						
			}
		}
		
	}
	
	/**
	 * Scans the application home directory and PHP include path
	 * and builds the component and administration page map that
	 * describe the configuration of the application..
	 */
	function scan($verbosity = 0)
	{
		global $config;
		
		Fakoli::$scanning = true;
		ComponentManager::$verbosityLevel = $verbosity;
		
		$this->tx = new DataTransaction();
		
		try
		{
			
			$c = new Component();
			$c->joinTransaction($this->tx);
			$c->deleteAll();
		
		
			$p = new AdminPage();
			$p->joinTransaction($this->tx);
			$p->deleteAll();
		
			$cp = new ComponentPage();
			$cp->joinTransaction($this->tx);
			$cp->updateExplicit("SET scanned=0");
			
			$path .= $config['homedir'] . PATH_SEPARATOR . ini_get("include_path");
			
			if (isset($config["extra_includes"])) $path .= PATH_SEPARATOR . $config["extra_includes"];
			
			$roots = explode(PATH_SEPARATOR, $path);
			
			foreach($roots as $root)
			{
				if ($root == ".") continue;
				
				ComponentManager::scanTrace("Scanning $root for components", 2);
				$this->scanComponents($root, "cms/components");
				$this->scanComponents($root, "components");
			}
			
			Cache::invalidate("fakoli_components");
			Cache::invalidate("fakoli_admin_menu");
			Cache::invalidate("fakoli_admin_icons");
			Cache::invalidate("fakoli_scripts");
			Cache::invalidate("fakoli_styles");
			Cache::invalidate("fakoli_event_map");
			
			$cp->delete("WHERE scanned=0");
			
			
			$this->tx->commit();
			unset($this->tx);
		
			$components = indexedQuery('Component', "WHERE enabled=1", "name");			
			Cache::put("fakoli_components", $components);
			
			ComponentManager::fireEvent("upgradeComponent");
			
			ComponentManager::fireEvent("ComponentScanComplete");
			
			Fakoli::$scanning = false;
		}
		catch(Exception $e)
		{
			// JDG 6/1/2011 - if fails on ComponentScanComplete
			if(isset($this->tx))
				$this->tx->rollback();
			unset($this->tx);
			
			Fakoli::$scanning = false;
			
			if (ComponentManager::$verbosityLevel > 0)
			{
				echo $e->__toString();
			}
			else
			{
				throw $e;
			}
		}
	}

	/**
	 * Locate the component path for the named component using the
	 * standard precedence rules.
	 * @param string $name
	 */
	static function findComponentPath($name)
	{
		global $config;
		
		$path .= $config['homedir'] . PATH_SEPARATOR . ini_get("include_path");

		if (isset($config["extra_includes"])) $path .= PATH_SEPARATOR . $config["extra_includes"];
		
		$roots = explode(PATH_SEPARATOR, $path);
			
		foreach($roots as $root)
		{
			if ($root == ".") continue;

			$path = $root . DIRECTORY_SEPARATOR . "cms" . DIRECTORY_SEPARATOR . "components" . DIRECTORY_SEPARATOR . $name;
			
			if (file_exists($path))
			{
				if (!file_exists($path.DIRECTORY_SEPARATOR."DEPRECATED")) return $path;
			}
			
			$path = $root . DIRECTORY_SEPARATOR . "components" . DIRECTORY_SEPARATOR . $name;
			
			if (file_exists($path)) 
			{
				if (!file_exists($path.DIRECTORY_SEPARATOR."DEPRECATED")) return $path;
			}
		}
		
		return null;
	}
	
	/**
	 * Retrieve the component definition record for the specified component.
	 * @param $component the name of the component
	 * @return Component the record for the specified component
	 */
	function getComponent($component)
	{
		$components = Fakoli::getComponents();
		
		if (!array_key_exists($component, $components))
		{
			throw new FakoliException("Unrecognized or disabled component '$component'");
		}
		return $components[$component];
	}	

	/**
	 * Retrieve the event handler map. The map is generated on the first request
	 * and cached for future reference.
	 * @return array the event handler map
	 */
	static function getEventHandlers()
	{
		$eventMap = Cache::get("fakoli_event_map");
		if ($eventMap) return $eventMap;
		
		ComponentManager::scanTrace("Mapping Event Handlers", 3);
		$components = Query::create('Component', "WHERE enabled=1 ORDER BY priority")->execute();
		
		$eventMap = array();
		
		foreach($components as $component)
		{
			$manifest = $component->loadManifest();
			
			if (!$manifest || !method_exists($manifest, subscribeToEvents)) continue;
	
			$subscriptions = $manifest->subscribeToEvents();
			
			foreach($subscriptions as $event => $handler)
			{
				$handlerRecord = array("component" => $component->name, "priority" => $component->priority, "handler" => $handler);
				
				ComponentManager::scanTrace("  $component->name is subscribing to $event", 3);
				if (array_key_exists($event, $eventMap))
				{
					$eventMap[$event][] = $handlerRecord;
				}
				else
				{
					$eventMap[$event] = array($handlerRecord);
				} 
			}
		}
		
		Cache::put("fakoli_event_map", $eventMap);
		return $eventMap;
	}
	
	/**
	 * Priority comparison callback - not for external use.
	 * @param array $item1
	 * @param array $item2
	 * @return integer comparison value
	 */
	static function compareByPriority($item1, $item2)
	{
		return $item1["priority"] - $item2["priority"];
	}
	
	/**
	 * Fire an event to all subscribers as detailed in their manifests.
	 * @param string $event the name of the event
	 * @param array $parameter any parameters being passed to the event
	 * @param boolean $mustBeConsumed if set to true, throws FakoliEventNotConsumed if no event handler
	 * has indicated consumption of the event by setting $continue to false
	 */
	static function fireEvent($event, $parameter = null, $mustBeConsumed = false)
	{
		trace("** Firing event '$event'", 4);
		 
		$eventMap = ComponentManager::getEventHandlers();
		
		if (!array_key_exists($event, $eventMap)) 
		{
			trace("No handlers subscribed to '$event'", 4);
			return $parameter;
		}
	
		$continue = true;
		
		foreach($eventMap[$event] as $subscription)
		{
			Fakoli::using($subscription["component"]);

			if (is_array($subscription["handler"]))
			{
				trace("Handling $event with {$subscription["handler"][0]}::{$subscription["handler"][1]}", 4);
			}
			else
			{
				trace("Handling $event with {$subscription["handler"]}", 4);
			}
					
			if (!is_callable($subscription["handler"]))
			{
				if (is_array($subscription["handler"]))
				{
					trace("Subscription handler {$subscription["handler"][0]}::{$subscription["handler"][1]} for $event is invalid", 2);
				}
				else
				{
					trace("Subscription handler $event with {$subscription["handler"]} for $event is invalid", 2);
				}

				trace(printIncludes(false, true), 3);
			}
			else
			{
				$parameter = call_user_func_array($subscription["handler"], array($parameter, &$continue));
			}
			if (!$continue) break;
		}
		
		if ($mustBeConsumed && $continue)
		{
			throw new FakoliEventNotConsumed("'$event' not handled by any subscriber");
		}
		
		return $parameter;
	}

	/**
	 * Fire an event to the specificed component. If the component is not subscribed
	 * the event is ignored.
	 * @param string $event the name of the event
	 * @param string $component The name of the target component
	 * @param array $parameter any parameters being passed to the event
	 */
	static function fireEventTo($event, $component, $parameter = null)
	{
		trace("** Firing event '$event' to $component", 4);
		 
		$eventMap = ComponentManager::getEventHandlers();
		
		if (!array_key_exists($event, $eventMap)) 
		{
			trace("No handlers subscribed to '$event'", 4);
			return $parameter;
		}
			
		foreach($eventMap[$event] as $subscription)
		{
			if ($component == $subscription["component"])
			{
				
				Fakoli::using($subscription["component"]);
	
				if (is_array($subscription["handler"]))
				{
					trace("Handling $event with {$subscription["handler"][0]}::{$subscription["handler"][1]}", 4);
				}
				else
				{
					trace("Handling $event with {$subscription["handler"]}", 4);
				}
						
				$parameter = call_user_func_array($subscription["handler"], array($parameter, &$continue));
				break;
			}
		}
		
		return $parameter;	
	}
	
	/**
	 * Dispatch an incoming user action to the appropriate component handler script.
	 * 
	 * @param string $process (optional) process identifier if running as a background process
	 */
	function dispatchAction($process = null)
	{
		global $script;
		global $isAction;
		$isAction = true;
		
		$component = $this->getComponent($_REQUEST["component"]);
		$action = $_REQUEST["action"];
			
		if (!preg_match("/^[\\w\\d_\\.]+$/", $action))
		{
			throw new FakoliException("Invalid action");
		}
				
		$this->fireEventTo("PreAction", $component->name, $action);
		
		ob_start();
		
		$this->dispatchActionInternal($process);
		
		$out = ob_get_contents();
		ob_end_clean();
		
		echo $script;
		echo $out;
		
		$this->fireEventTo("PostAction", $component->name, $action);
		
		ComponentManager::fireEvent("RequestComplete");
	}
	
	private function dispatchActionInternal($process = null)
	{
		global $method;
		global $user;
		global $config;
		global $script;
		
		$action = $_REQUEST["action"];
		
		trace("ComponentManager::dispatchAction action: $action", 3);
		
		$component = $this->getComponent($_REQUEST["component"]);
		
		if (!preg_match("/^[\\w\\d_\\.]+$/", $action))
		{
			throw new FakoliException("Invalid action");
		}
		
		$manifest = $component->loadManifest();
		
		if (!isset($user) && !$process && (!$manifest->allow_sessionless_handlers || 
				(is_array($manifest->allow_sessionless_handlers) && array_search($action, $manifest->allow_sessionless_handlers) === FALSE)))
		{
			throw new FakoliException("Session Expired");
		}

		$handler = $component->component_path . DIRECTORY_SEPARATOR . "handlers" . DIRECTORY_SEPARATOR . $action . ".inc";
		trace("ComponentManager::dispatchAction handler: $handler", 3);
		
		Fakoli::applicationTraceLevel();
			
		if (!is_file($handler))
		{
			throw new FakoliException("Invalid action");
		}
		
		try
		{
			include $handler;
		}
		catch(FakoliEarlyExit $exit)
		{
			echo $exit->getMessage();
		}		

		Fakoli::coreTraceLevel();
		
	}
	
	function dispatchLocalAction($action, $process = null)
	{
		global $method;
		
		trace($action, 3);
		
		list($uri, $queryString) = explode("?", $action);
		
		trace("URI: $uri, Query String: $queryString", 3);
		
		list($empty, $prefix, $component, $action) = explode("/", $uri);
		
		trace("Prefix: $prefix, Component: $component, Action: $action", 3);
		
		if ($prefix != "action") 
		{
			throw new FakoliException("Malformed action handler");
		}

		$context = array(
			"_GET" 		=> serialize($_GET), 
			"_REQUEST" 	=> serialize($_REQUEST), 
			"_SERVER"	=> serialize($_SERVER),
			"method" 	=> serialize($method)
		);
		
		parse_str($queryString, $_GET);
		
		//trace("\$_GET: " . json_encode($_GET), 3);
		
		$_SERVER["REQUEST_URI"] = $uri;
		
		$_GET["component"] = $component;
		$_GET["action"] = $action;
		
		$_REQUEST["component"] = $component;
		$_REQUEST["action"] = $action;	
		
		ob_start();
		
		try
		{
			$this->dispatchActionInternal($process);
		}
		catch(Exception $e)
		{
			trace($e->getMessage(), 3);
			echo $e->getMessage();
		}
		
		$output = ob_get_contents();
		ob_end_clean();

		//trace(print_r($context, true), 3);

		trace(print_r($context, true), 3);
		
// 		foreach($context as $name => $value)
// 		{
// 			$$name = unserialize($value);
			
// 			trace("$name: ".print_r($$name, true), 3);
// 		}
		
		$_GET = unserialize($context["_GET"]);
		$_REQUEST = unserialize($context["_REQUEST"]);
		$_SERVER = unserialize($context["_SERVER"]);
		$methood = unserialize($context['method']);
		
		trace("URI: ".$_SERVER["REQUEST_URI"], 3);
		return $output;
	}
	
	static function displayPage($identifier, &$continue)
	{
		global $page_role;
		
		try
		{
			checkIdentifier($identifier);
			
			$page = ComponentPage::findByIdentifier($identifier, "WHERE enabled=1");
			
			// JDG 11/2011 - are templates ever stored with tpl at the end?? 
			$page_template = (preg_match("/tpl$/", $page->template)) ? $page->template : "{$page->template}.tpl";
			$pageView = new ComponentPageView($page, $page_template);
		
			$page_role = $page->role;
			
			if (!checkRole($page->role))
			{
				LoginManager::storeRedirectPage();
				redirect("/login");
			}
			
			echo $pageView->drawView();
			
			$continue = false;
		}
		catch(DataNotFoundException $e)
		{
			
		}
		
		return $identifier;
	}
	
	static function enumerateComponentPages($items)
	{
		$pages = Query::create('ComponentPage', "ORDER BY identifier")->execute();
		
		$items["Component Pages"] = $pages;
		return $items;
	}
	
	static function deleteRole($del_role)
	{
		$constraint = "WHERE role like '%$del_role%'";
		$componentPages = Query::create('ComponentPage', $constraint)->execute();
		
		RoleManager::deleteRoleFromString($componentPages, $del_role);

		return $del_role;
		
	}
	
	static function setComponentVersion($component_name)
	{
		$components = Query::create('Component', "WHERE name=:component_name")
			->bind(":component_name", $component_name)
			->execute();
		
		if(count($components) == 0)
			return;
			
		$component = $components[0];
		
		$latest_version = ComponentUpdateLog::getLatestVersion($component_name);
		
		// To do - set inital values in table for existing components to start

		if($latest_version && $latest_version != $component->version)
		{
			$component->version = $latest_version;
			$component->filter = new InclusionFilter("version");
			$component->save();	
		}
		
	}
	
	static function upgradeComponent($version)
	{
		$mgr = new ComponentUpgradeManager();
		$mgr->upgrade($version);
	}	
	

	static function componentListTabs()
	{
		$tabs = array(	"Components"		=>	"/admin/components",
					"Component Pages"	=>	"/admin/component_pages"
					);

		$bar = new TabBar("tabs", $tabs, "");
		$bar->useQueryString = false;
		
		return $bar;
	}

	static function componentPageTabs($key)
	{
		$tabs = array(	"Definition"	=>	"/admin/component_page_form",
					"Modules"		=>	"/admin/component_page_modules");
	
		$qs = ($key) ? "component_page_id=$key" : ""; 
		$bar = new TabBar("tabs", $tabs, $qs);
		
		return $bar;
	}

	static function componentFormTabs($key)
	{
		$tabs = array(	"Definition"		=>	"/admin/component_form",
					"Component Pages"	=>	"/admin/component_pages",
					"Version History"	=>	"/admin/component_versions"
		);
	
		$qs = ($key) ? "component_id=$key" : ""; 
		$bar = new TabBar("tabs", $tabs, $qs);
		
		return $bar;
	}
	
	static function setDefaults()
	{
		trace("ComponentManager::setDefaults", 3);
		global $config;
		
		$defaultPath = $config['uploadbase'] . DIRECTORY_SEPARATOR . "upgrade_output_path";		
		if (!file_exists($defaultPath)) mkdir($defaultPath);
		
		Settings::setDefaultValue("component", "upgrade_output_path", $defaultPath, "String");
		Settings::setDefaultValue("settings", "enable_inline_editing", false, "Boolean", "Turns on or off inline editing capabilities (for those components that support this option)", "General");
		Settings::setDefaultPermission("settings", "editor_roles", "Roles for whom inline editing controls are to be shown", "admin"); 
	}
	
	/**
	 * Retrieves a unique signature for the application's enabled components,
	 * based on their installed version numbers. This enables the administrator
	 * to determine at a glance if the configuration of two Fakoli servers is
	 * the same. 
	 */
	static function getComponentSignature()
	{
		$str = "";
		
		$components = Fakoli::getComponents();
		foreach($components as $component)
		{
			$version = ComponentUpdateLog::getLatestVersion($component->name);
			$str .= $component->name.":".$version."|";
		}
		
		return sha1($str);
	}
	
}

/**@}*/
?>
