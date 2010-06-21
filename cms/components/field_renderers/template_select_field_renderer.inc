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

require_once realpath(dirname(__FILE__))."/../../../framework/field_renderers.inc";

class TemplateSelectFieldRenderer extends SelectFieldRenderer
{
	function TemplateSelectFieldRenderer(&$form, $field, $label, $directory, $onChange="")
	{
		global $config;
		$path = $config["homedir"]."/".$directory;
	
		$options = array();
		$options = array_merge($options, $this->readFiles("{$config["homedir"]}/$directory", ""));
			
		$this->SelectFieldRenderer($form, $field, $label, $options, $onChange);
	}
	
	function readFiles($home, $dir)
	{
		trace("Scanning Templates", 3);
		 
		$options = array();
		
		$folders = array("$dir");		
		
		while(($dir = array_shift($folders)) !== null)
		{
			$path = "$home/$dir";
			
			trace("Reading $path", 3);

			$handle = opendir($path);
			while(false !== ($file = readdir($handle))) 
	    	{
	    		trace("Checking $file", 4);
	    		
	    		if ($file != "." && 
	    			$file != ".." && 
	    			$file != ".svn" && 
	    			$file != "CVS")
	    		{
	    			$f = "$path/$file";
					$l = ($dir != '') ? "$dir/$file" : $file;
					
	    			if (is_dir($f))
	    			{
	    				array_push($folders, $l);
	    			}
					else if (preg_match('/.tpl$/i', $file)) 
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
