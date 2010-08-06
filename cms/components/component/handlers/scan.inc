<?php

function scanComponents($root, $subdir, $tx)
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
	    		
	    		if (!is_file($m))
	    		{
	    			trace("== No manifest found. Skipping...", 3);
	    			continue;
	    		}
	    		
	    		require_once $m;
	    		
	    		$cl = getComponentClassRoot($file) . "Manifest";
	    		
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
	    		$component->joinTransaction($tx);
	    		$component->save();
	    		
	    		scanAdminPages($component, $tx);
	    		
	    		Cache::invalidate("fakoli_includes_$file");
	    	}
	    }
	    
	    closedir($handle);
	}
}

function scanAdminPages($component, $tx)
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
			$page->joinTransaction($tx);
			$page->save();
		}
	}
}

function getComponentClassRoot($component)
{
	return str_replace(" ", "", prettify($component));
}

global $config;

$tx = new DataTransaction();

try
{
	
	$c = new Component();
	$c->joinTransaction($tx);
	$c->deleteAll();


	$p = new AdminPage();
	$p->joinTransaction($tx);
	$p->deleteAll();

	$path = ini_get("include_path");
		
	$path .= PATH_SEPARATOR . $config['homedir'];
	
	$roots = explode(PATH_SEPARATOR, $path);
	
	foreach($roots as $root)
	{
		scanComponents($root, "cms/components", $tx);
		scanComponents($root, "components", $tx);
	}
	
	Cache::invalidate("fakoli_components");
	Cache::invalidate("fakoli_admin_menu");
	
	$tx->commit();
	
	redirect("/admin/components");
}
catch(Exception $e)
{
	$tx->rollback();
	throw $e;
}
?>