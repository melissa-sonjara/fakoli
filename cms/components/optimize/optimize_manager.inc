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

Fakoli::using("settings", "optimize");
Fakoli::usingFeature("cache");

class OptimizeManager
{
	static $zipped = false;
	
	static function clearCache()
	{
		Cache::invalidate("script_cache");
		Cache::invalidate("script_cache_timestamp");
		Cache::invalidate("css_cache");
		Cache::invalidate("css_cache_timestamp");
		Settings::setDefaultValue("optimize", "optimize_scripts", "", Boolean);
		Settings::setDefaultValue("optimize", "optimize_styles", "", Boolean);
		Settings::setDefaultValue("optimize", "compress_javascript", "", Boolean);
		Settings::setDefaultValue("optimize", "compress_styles", "", Boolean);
		Settings::setDefaultValue("optimize", "compress_page_content", "", Boolean);
		Settings::setDefaultValue("optimize", "append_timestamp", false, Boolean, "Specifies whether to include a timestamp in the generated link for optimized scripts and styles");
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
						$cachedScripts .= "\r\n";
						trace("Added $script", 3);
					}
					else
					{
						trace("Could not resolve script $script", 2);
					}
				}
				
				Cache::put("script_cache", $cachedScripts);
				Cache::put("script_cache_timestamp", $last_modified);
			}
			
			$scriptLink = Settings::getValue("optimize", "append_timestamp") ? "/action/optimize/scripts?".Cache::get("script_cache_timestamp") : "/action/optimize/scripts";
				
			$scripts = array($scriptLink);
		}
			
		return $scripts;
	}
		
	static function getScriptsContent()
	{
		$content = Cache::get("script_cache");
		if (!$content)
		{
			$scripts = Fakoli::loadScripts();
			OptimizeManager::preprocessScripts($scripts);	
			$content = Cache::get("script_cache");
		}
		
		return $content;
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
						$cachedStyles .= "\r\n";
					}
				}
				
				$cachedStyles = preg_replace("/^@CHARSET.*$/mi", "", $cachedStyles);
				$cachedStyles = str_replace("\r\n", "\n", $cachedStyles);	
				$cachedStyles = str_replace("\n", "\r\n", $cachedStyles);	
				
				Cache::put("css_cache", $cachedStyles);
				Cache::put("css_cache_timestamp", $last_modified);
			}
			
			$styleLink = Settings::getValue("optimize", "append_timestamp") ? "/action/optimize/styles?".Cache::get("css_cache_timestamp") : "/action/optimize/styles";
			
			$styles = array($styleLink);
		}
			
		return $styles;
	}
	
	static function getStylesContent()
	{
		$cachedStyles = Cache::get("css_cache");
			
		if (!$cachedStyles)
		{
			$styles = Fakoli::loadStyles();
			OptimizeManager::preProcessStyles($styles);
			$cachedStyles = Cache::get("css_cache");			
		}
		
		return $cachedStyles;
	}
	
	static function preProcessPage($template)
	{
		if (Settings::getValue("optimize", "compress_page_content") && substr_count($_SERVER['HTTP_ACCEPT_ENCODING'], 'gzip')) 
		{
			ob_start("ob_gzhandler");
			$level = ob_get_level();
			trace("ZIP output buffer level: $level", 3);
			Fakoli::$outputBufferLevel = $level;
		}
		return $template;
	}
	
	static function preAction($action)
	{
		trace("PreAction: $action", 3);
		switch($action)
		{
			case "scripts":
				if (Settings::getValue("optimize", "compress_javascript") && substr_count($_SERVER['HTTP_ACCEPT_ENCODING'], 'gzip')) 
				{
					trace("Compressing scripts", 3);
					OptimizeManager::$zipped = true;	
				}
				break;
	
			case "styles":
					
				if (Settings::getValue("optimize", "compress_styles") && substr_count($_SERVER['HTTP_ACCEPT_ENCODING'], 'gzip')) 
				{
					trace("Compressing styles", 3);
					OptimizeManager::$zipped = true;	
				}
				break;
				
			default:
		}
	}
	
	static function startZip()
	{
		if (OptimizeManager::$zipped)
		{
			ob_start();
			if (!ob_start("ob_gzhandler"))
			{
				trace("Zip handler ignored", 3);
			}
		}
	}
	
	static function flushZip()
	{
		if (OptimizeManager::$zipped)
		{
			trace("Flushing zipped content", 3);
			trace(print_r(ob_list_handlers(), true), 3);
			ob_end_flush();
			header("Content-Length: ".ob_get_length());
			ob_end_flush();
			trace(print_r(ob_list_handlers(), true), 3);
		}
	}
	
	static function postAction($action)
	{
	}
}
