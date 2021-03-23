<?php
Fakoli::using("component");

class ModuleDocTagsManager
{
	static function scanTrace($msg, $level = 2)
	{
		global $process;
		if ($process)
		{
			echo $msg."\n";
		}
		else
		{
			echo "<p>$msg</p>\n";
		}
	}
	
	static function scanComponents($root, $subdir)
	{
		if (!$root || $root == ".") return;
		
		$path = $root . DIRECTORY_SEPARATOR . str_replace("/", DIRECTORY_SEPARATOR, $subdir);
		
		ModuleDocTagsManager::scanTrace("Scanning $path", 2);
		
		if (is_dir($path))
		{
			$handle = opendir($path);
			while(false !== ($file = readdir($handle)))
			{
				$f = $path . DIRECTORY_SEPARATOR . $file;
				
				ModuleDocTagsManager::scanDirectory($f);
			}
			
			closedir($handle);
		}
	}

	static function scanDirectory($f)
	{
		$m = $f . DIRECTORY_SEPARATOR . "manifest.inc";
		$d = $f . DIRECTORY_SEPARATOR . "DEPRECATED";
		
		$datamodel = $f . DIRECTORY_SEPARATOR . "datamodel";
		
		if (is_dir($f))
		{
			ModuleDocTagsManager::scanTrace("== Found component '$file'", 3);
			if (is_file($d))
			{
				ModuleDocTagsManager::scanTrace("**** Deprecated component - ignoring", 3);
				return;
			}
			
			if (!is_file($m))
			{
				ModuleDocTagsManager::scanTrace("== No manifest found. Skipping...", 3);
				return;
			}
			
			$componentName = basename($f);
			$componentPrettyName = prettify($componentName);
			
			$contents = file_get_contents($m);
			if (preg_match("/\\\\defgroup/", $contents))
			{
				
				ModuleDocTagsManager::scanTrace("Module group already defined in manifest", 3);
			}
			else
			{
				$contents = preg_replace("/^<\\?p?h?p?/s", "<?php\n/**\defgroup {$componentName} {$componentPrettyName}\n * @{\n */\n\n", $contents);
				if (preg_match("/\\?>\\s*$/s", $contents))
				{
					$contents = preg_replace("/\\?>\\s*$/s", "/**@}*/?>", $contents);
				}
				else
				{
					$contents .= "\n/**@}*/?>";
				}
				
				ModuleDocTagsManager::scanTrace("Processed Manifest");
				file_put_contents($m, $contents);
				echo "<pre>".htmlentities($contents)."</pre>\n";
			}
			
			
			ModuleDocTagsManager::scanFiles($componentName, $f);
			if (file_exists($datamodel))
			{
				ModuleDocTagsManager::scanFiles($componentName, $datamodel);
			}
		}
	}
	
	static function scanFiles($component, $dir)
	{
		$handle = opendir($dir);
		while (false !== ($file = readdir($handle)))
		{
			if (!endsWith($file, ".inc"))
			{
				continue;
			}
			
			if ($file == "manifest.inc")
			{
				continue;
			}
			
			$f = $dir . DIRECTORY_SEPARATOR . $file;
			
			$contents = file_get_contents($f);
			if (preg_match("/\\\\addtogroup/", $contents))
			{
				ModuleDocTagsManager::scanTrace("Module group already present in $file - skipping");
				continue;
			}
			
			$contents = preg_replace("/^<\\?p?h?p?/s", "<?php\n/**\addtogroup {$component}\n * @{\n */\n\n", $contents);
			
			if (preg_match("/\\?>\\s*$/s", $contents))
			{
				$contents = preg_replace("/\\?>\\s*$/s", "/**@}*/?>", $contents);
			}
			else
			{
				$contents .= "\n/**@}*/?>";
			}
			
			echo "<h3>$file</h3>\n";
			file_put_contents($f, $contents);
			echo "<pre>".htmlentities($contents)."</pre>";
		}
	}
		
		
	static function scan()
	{
		$path .= $config['homedir'] . PATH_SEPARATOR . ini_get("include_path");
		
		if (isset($config["extra_includes"])) $path .= PATH_SEPARATOR . $config["extra_includes"];
		
		$roots = explode(PATH_SEPARATOR, $path);
		
		foreach($roots as $root)
		{
			if ($root == ".") continue;
			
			ModuleDocTagsManager::scanTrace("Scanning $root for components", 2);
			ModuleDocTagsManager::scanComponents($root, "cms/components");
			ModuleDocTagsManager::scanComponents($root, "components");
		}
	}
}