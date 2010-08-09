<?php

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
		
			$path .= $config['homedir'] . PATH_SEPARATOR . ini_get("include_path");
			
			$roots = explode(PATH_SEPARATOR, $path);
			
			foreach($roots as $root)
			{
				$this->scanComponents($root, "cms/components");
				$this->scanComponents($root, "components");
			}
			
			Cache::invalidate("fakoli_components");
			Cache::invalidate("fakoli_admin_menu");
			
			$this->tx->commit();
			unset($this->tx);
		}
		catch(Exception $e)
		{
			$tx->rollback();
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
		$components = Cache::get("fakoli_components");
		if (!$components)
		{
			$components = Fakoli::getComponents();
		}
		
		if (!array_key_exists($component, $components))
		{
			throw new FakoliException("Unrecognized or disable component '$component'");
		}
		return $components[$component];
	}	

	function dispatchEvent()
	{
		$component = $this->getComponent($_REQUEST["component"]);
		$action = $_REQUEST["action"];
		
		if (!preg_match("/^[\\w\\d_]+$/", $action))
		{
			throw new FakoliException("Invalid action");
		}
		
		$handler = $component->component_path . DIRECTORY_SEPARATOR . "handlers" . DIRECTORY_SEPARATOR . $action . ".inc";
		
		if (!is_file($handler))
		{
			throw new FakoliException("Invalid action");
		}
		
		include $handler;
	}
}
?>