<?php
require_once realpath(dirname(__FILE__))."/../../../framework/field_renderers.inc";

class CodeFileSelectFieldRenderer extends SelectFieldRenderer
{
	function CodeFileSelectFieldRenderer(&$form, $field, $label, $directory, $onChange="")
	{
		global $config;
		$path = $config["homedir"]."/".$directory;
	
		$options = array("" => "");
		
		if ($handle = opendir($path)) 
		{
	    	while (false !== ($file = readdir($handle))) 
	    	{
	  	       	if (preg_match('/.inc$/i', $file)) 
	 	       	{
	 	       		$options[$file] = $file;
	 	       	}        	
	    	}
	
		    closedir($handle);		
		}
		
		$this->SelectFieldRenderer($form, $field, $label, $options, $onChange);
	}
}
?>
