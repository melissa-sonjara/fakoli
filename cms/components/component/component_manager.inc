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
	
	/**
	 * Creates a new ComponentManager object
	 */
	function ComponentManager()
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
	
		trace("Scanning $path", 3);
		
		if (is_dir($path))
		{
			$handle = opendir($path);
			while(false !== ($file = readdir($handle))) 
		    {
		    	$f = $path . DIRECTORY_SEPARATOR . $file;
		    	$m = $f . DIRECTORY_SEPARATOR . "manifest.inc";
		    	
		    	if (is_dir($f))
		    	{
		    		trace("== Found component '$file'", 3);
		    		if ($this->found[$file])
		    		{
		    			trace("---- already defined - skipping", 3);
		    			continue;
		    		}
		    		
		    		if (!is_file($m))
		    		{
		    			trace("== No manifest found. Skipping...", 3);
		    			continue;
		    		}
		    		
		    		require_once $m;
		    		
		    		$cl = Fakoli::getComponentClassRoot($file) . "Manifest";
		    		
		    		try
		    		{
			    		trace("== Instantiating $cl", 3);
			    		
			    		$manifest = new $cl;
		    		}
		    		catch(Exception $e)
		    		{
		    			trace("-- Failed to instantiate manifest class. Skipping...", 3);	    		
		    		}
	
		    		$component = $manifest->getComponentDefinition();
		    		$component->component_path = $f;
		    		$component->joinTransaction($this->tx);
		    		$component->save();
		    		
		    		$this->found[$file] = $component;
		    		
		    		$this->scanAdminPages($component);
		    		$this->scanComponentPages($component);
		    		
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
		$admin = $component->component_path . DIRECTORY_SEPARATOR . "admin";
		
		if (!is_dir($admin)) return;
		
		trace("== Scanning $admin", 3);
		
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
		trace("== Scanning $pages", 3);
		
		$handle = opendir($pages);
		
		while(false !== ($file = readdir($handle))) 
		{
			if (endsWith($file, ".inc"))
			{
				$identifier = preg_replace("/\\.inc$/", "", $file);
				
				try
				{
					$page = Query::create(ComponentPage, "WHERE component=:c AND identifier=:i")
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
					$page->enabled = false;
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
	function scan()
	{
		global $config;

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
			
			$roots = explode(PATH_SEPARATOR, $path);
			
			foreach($roots as $root)
			{
				$this->scanComponents($root, "cms/components");
				$this->scanComponents($root, "components");
			}
			
			Cache::invalidate("fakoli_components");
			Cache::invalidate("fakoli_admin_menu");
			Cache::invalidate("fakoli_scripts");
			Cache::invalidate("fakoli_styles");
			Cache::invalidate("fakoli_event_map");
			
			$cp->delete("WHERE scanned=0");
			
			
			$this->tx->commit();
			unset($this->tx);
		
			ComponentManager::fireEvent("ComponentScanComplete");
		}
		catch(Exception $e)
		{
			$this->tx->rollback();
			unset($this->tx);
			throw $e;
		}
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
			throw new FakoliException("Unrecognized or disable component '$component'");
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
		
		trace("Mapping Event Handlers", 3);
		$components = Query::create(Component, "WHERE enabled=1 ORDER BY priority")->execute();
		
		$eventMap = array();
		
		foreach($components as $component)
		{
			$manifest = $component->loadManifest();
			
			if (!method_exists($manifest, subscribeToEvents)) continue;
	
			$subscriptions = $manifest->subscribeToEvents();
			
			foreach($subscriptions as $event => $handler)
			{
				$handlerRecord = array("component" => $component->name, "priority" => $component->priority, "handler" => $handler);
				
				trace("  $component->name is subscribing to $event", 3);
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
	 */
	static function fireEvent($event, $parameter = null)
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
					
			$parameter = call_user_func_array($subscription["handler"], array($parameter, &$continue));
			if (!$continue) break;
		}
		
		return $parameter;
	}
		 
	/**
	 * Dispatch an incoming user action to the appropriate component handler script.
	 */
	function dispatchAction()
	{
		global $method;
		global $user;
		global $config;
				
		$component = $this->getComponent($_REQUEST["component"]);
		$action = $_REQUEST["action"];
		
		trace("ComponentManager::dispatchAction action: $action", 4);
		
		if (!preg_match("/^[\\w\\d_\\.]+$/", $action))
		{
			throw new FakoliException("Invalid action");
		}
		
		$handler = $component->component_path . DIRECTORY_SEPARATOR . "handlers" . DIRECTORY_SEPARATOR . $action . ".inc";
		trace("ComponentManager::dispatchAction handler: $handler", 4);
		
		// JDG tweak of desperation
		//if($handler == "\handlers\scan.inc")
			//$handler = "c:\code\\fakoli_2.0\cms\components\component\handlers\scan.inc";
			
		if (!is_file($handler))
		{
			throw new FakoliException("Invalid action");
		}
		
		include $handler;
	}
	
	static function displayPage($identifier, &$continue)
	{
		global $page_role;
		
		try
		{
			$page = ComponentPage::findByIdentifier($identifier, "WHERE enabled=1");
			$pageView = new ComponentPageView($page, "{$page->template}.tpl");
		
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
		$pages = Query::create(ComponentPage, "ORDER BY identifier")->execute();
		
		$items["Component Pages"] = $pages;
		return $items;
	}
	
	static function deleteRole($del_role)
	{
		$constraint = "WHERE role like '%$del_role%'";
		$componentPages = Query::create(ComponentPage, $constraint)->execute();
		
		RoleManager::deleteRoleFromString($componentPages, $del_role);

		return $del_role;
		
	}
}
?>