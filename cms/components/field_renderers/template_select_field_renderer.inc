<?php
require_once realpath(dirname(__FILE__))."/../../../framework/field_renderers.inc";

class TemplateSelectFieldRenderer extends SelectFieldRenderer
{
	function TemplateSelectFieldRenderer(&$form, $field, $label, $directory, $onChange="")
	{
		global $config;
		$path = $config["homedir"]."/".$directory;

		trace("Scanning $path", 3);
		
		$options = array();
		
		if ($handle = opendir($path)) 
		{
	    	while (false !== ($file = readdir($handle))) 
	    	{
	  	       	if (preg_match('/.tpl$/i', $file)) 
	 	       	{
	 	       		$options[$file] = prettify(str_replace(".tpl", "", $file));
	 	       		trace("Found $file", 3);
	 	       	}        	
	    	}
	
		    closedir($handle);		
		}
		
		$this->SelectFieldRenderer($form, $field, $label, $options, $onChange);
	}
}
?>
