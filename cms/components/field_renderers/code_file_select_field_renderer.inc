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
		$options = array();
		
		$path = "$home/$dir";
		$handle = opendir($path);
		while(false !== ($file = readdir($handle))) 
    	{
    		if ($file != "." && 
    			$file != ".." && 
    			$file != ".svn" && 
    			$file != "CVS")
    		{
    			$f = "$path/$file";
				$l = "$dir/$file";
				
    			if (is_dir($f))
    			{
    				$options[$l] = readFile($home, $l);	
    			}
				else if (preg_match('/.inc$/i', $file)) 
				{
					$options[$l] = $l;
				}
    		}        	
    	}

	    closedir($handle);
	    return $options;
	}
}
?>
