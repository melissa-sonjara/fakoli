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

Fakoli::using("settings");
Fakoli::usingFeature("cache");

class OptimizeManager
{
	static function clearCache()
	{
		Cache::invalidate("script_cache");
		Cache::invalidate("script_cache_timestamp");
		Cache::invalidate("css_cache");
		Cache::invalidate("css_cache_timestamp");
		Settings::setDefaultValue("optimize", "optimize_scripts", "");
		Settings::setDefaultValue("optimize", "optimize_styles", "");
	}
	
	static function preprocessScripts($scripts)
	{
		if (Settings::getValue("optimize", "optimize_scripts"))
		{
			$cachedScripts = Cache::get("script_cache");
			
			if (!$cachedScripts)
			{
				foreach($scripts as $script)
				{
					$resource = Fakoli::resolveResource($script);
					if ($resource)
					{
						$file_modified = filemtime($resource);
						if ($file_modified > $last_modified)
						{
							$last_modified = $file_modified;
						}
						
						$cachedScripts .= file_get_contents($resource);
						$cachedScripts .= "\n";
					}
				}
				
				Cache::put("script_cache", $cachedScripts);
				Cache::put("script_cache_timestamp", $last_modified);
			}
			
			$scripts = array("/action/optimize/scripts");
		}
			
		return $scripts;
	}
		
	static function preProcessStyles($styles)
	{
		if (Settings::getValue("optimize", "optimize_styles"))
		{
			$cachedStyles = Cache::get("css_cache");
			
			if (!$cachedStyles)
			{
				foreach($styles as $style)
				{
					$resource = Fakoli::resolveResource($style);
					if ($resource)
					{
						$file_modified = filemtime($resource);
						if ($file_modified > $last_modified)
						{
							$last_modified = $file_modified;
						}
						
						$cachedStyles .= file_get_contents($resource);
						$cachedStyles .= "\n";
					}
				}
				
				Cache::put("css_cache", $cachedStyles);
				Cache::put("css_cache_timestamp", $last_modified);
			}
						
			$styles = array("/action/optimize/styles");
		}
			
		return $styles;
	}
}
