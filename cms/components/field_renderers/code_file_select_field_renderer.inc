<?php
require_once realpath(dirname(__FILE__))."/../../../framework/field_renderers.inc";

class CodeFileSelectFieldRenderer extends SelectFieldRenderer
{
	function CodeFileSelectFieldRenderer(&$form, $field, $label, $directory, $onChange="")
	{
		global $config;
		$path = $config["homedir"]."/".$directory;
	
		$options = array("" => "");
		$options = array_merge($options, $this->readFiles($config["homedir"], $directory));
			
		$this->SelectFieldRenderer($form, $field, $label, $options, $onChange);
	}
	
	function readFiles($home, $dir)
	{
		trace("Scanning Modules", 3);
		 
		$options = array();
		
		$folders = array("$dir");		
		
		while(($dir = array_shift($folders)) !== null)
		{
			$path = "$home/$dir";
			
			trace("Reading $path", 3);
			
			$handle = opendir($path);
			while(false !== ($file = readdir($handle))) 
	    	{
	    		trace($file, 4);
	    		
	    		if ($file != "." && 
	    			$file != ".." && 
	    			$file != ".svn" && 
	    			$file != "CVS")
	    		{
	    			$f = "$path/$file";
					$l = "$dir/$file";
					
	    			if (is_dir($f))
	    			{
	    				array_push($folders, $l);	
	    			}
					else if (preg_match('/.inc$/i', $file)) 
					{
						$options[$l] = $l;
					}
	    		}        	
	    	}
	
		    closedir($handle);
		}
		
	    return $options;
	    
	}
}
?>
