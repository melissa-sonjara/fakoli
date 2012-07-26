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

Fakoli::usingFeature("field_renderers");
Fakoli::using("component");

class CodeFileSelectFieldRenderer extends SelectFieldRenderer
{
	function CodeFileSelectFieldRenderer(&$form, $field, $label, $directory, $onChange="")
	{
		trace("Scanning Modules", 3);
		
		global $config;
		$path = $config["homedir"]."/".$directory;
	
		$options = array("" => array("" => ""));
		$options = array_merge($options, array("General Code Files" => $this->readFiles($config["homedir"], $directory)));
		
		$components = Fakoli::getComponents();
		trace(count($components)." components", 3);
		foreach($components as $name => $component)
		{
			trace("Scanning $name", 3);
			$modules = $this->readFiles($component->component_path, $directory, $name);
			if (count($modules) > 0)
			{
				$options = array_merge($options, array(prettify($name)." Modules" => $modules));
			}
		}
		
		$this->SelectFieldRenderer($form, $field, $label, $options, $onChange, true);
	}
	
	function readFiles($home, $dir, $prefix = null)
	{		 
		$options = array();
		
		$folders = $this->getFilesInPath($options, $home, $dir, $prefix);
		if(count($folders) > 0)
		{
			foreach($folders as $folder)
			{
				trace("calling getFilesInPath for folder $folder", 3);
				$this->getFilesInPath($options, $home, $folder, $prefix);
			}
		}
	    return $options;
	    
	}
	
	function getFilesInPath(&$options, $home, $dir, $prefix)
	{
		$path = $home . DIRECTORY_SEPARATOR . $dir;
		
		trace("Reading $path", 3);
		
		if (!file_exists($path)) 
		{
			return $options;
		}
		
		$handle = opendir($path);
		$folders = array();
 
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
				trace("f is $f and l is $l", 3);
				
    			if (is_dir($f) AND $file != "include")
    			{
     				array_push($folders, $l);
   				}
				else if (preg_match('/.inc$/i', $file)) 
				{
					if ($prefix)
					{
						$options["$prefix,$l"] = $l;
					}
					else
					{
						$options[$l] = $l;
					}
				}
    		}        	
    	}
    	closedir($handle);
    	return $folders;
	}
}
?>