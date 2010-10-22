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

Fakoli::using("page", "menu", "module");

/**
 * PageView generates the page content, substituting page fields, menus and associated modules
 * into an HTML template.
 * 
 * @author andy
 */
class PageView
{
	var $page; 		///< The Page object that we are viewing
	var $template; 	///< The layout template used to render the page.
	
	function PageView($page, $template)
	{
		$this->page = $page;
		$this->template = $template;	
	}
	
	function drawView()
	{
		global $config;
		global $method;
		global $user;
		global $script;
		global $styles;
		global $breadcrumbs;
		global $dialogs;
		global $page;
		global $site;
		
		$page = $this->page; // Pass through to code include
		$site = $page->Site(); // Pass through to code include
		
		$template = $this->page->getTemplate();
		
		$template = ComponentManager::fireEvent("PreProcessPage", $template);
		
		// If the page has a PHP code include, execute it and insert the contents
		
		if ($this->page->php_code_file)
		{
			ob_start();
			include $config['homedir']."/{$this->page->php_code_file}";
			$output = ob_get_contents();
			ob_end_clean();
			
			$this->page->description .= $output;
		}
		
		$output = ComponentManager::fireEvent("PostProcessContent", $output);
		
		// Populate template fields from the Page object.
		
		foreach($this->page->fields as $field => $type)
		{
			$template = str_replace("{".$field."}", $this->page->$field, $template);
		}		
		
		$qs = $_SERVER['QUERY_STRING'];
		$identifier = $_REQUEST["identifier"];
		
		$qs = preg_replace("/identifier=[\\w_]+&*/", "", $qs);
		$qs = preg_replace("/_style=\\w+&*/", "", $qs);
		
		$printLink = "/{$this->page->identifier}".appendToQueryString($qs, "_style=print");
		
		$template = str_replace("{printIcon}", 
			"<a href='$printLink' target='_blank'><img src='/fakoli/images/print.gif' style='display: inline-block; vertical-align: middle;border: none'></a>&nbsp;<a href='$printLink' target='_blank'>Print this Page</a>",
			$template);
		
		// Populate the template with the appropriate menu(s).
		
		$menuPositions = array();
		
		preg_match_all("/\{menu:(.*?)\}/", $template, $menuPositions, PREG_SET_ORDER);
		
		$menus = indexedQuery(Menu, "WHERE site_id={$this->page->site_id}", "identifier");
		
		foreach($menuPositions as $position)
		{
			$output = "";
			
			trace("Substituting Menu at Position {$position[0]} {$position[1]}", 3);
						
			if (array_key_exists($position[1], $menus))
			{
				$output .= "<div id='$position[1]'>";
				
				$view = new MenuView($menus[$position[1]]);
				$output .= $view->drawView();	
				
				$output .= "</div>";			
			}
			
			$template = str_replace($position[0], $output, $template);			
		}
		
		
		
		// Populate the template with associated modules for the page at the correct positions.
		
		$positions = array();
		
		preg_match_all("/\{position:(.*?)\}/", $template, $positions, PREG_SET_ORDER);
		
		$modules = $this->page->getModulesByPosition();
	
		foreach($positions as $position)
		{
			$output = "";
			
			trace("Substituting at Position {$position[0]} {$position[1]}", 3);
						
			if (array_key_exists($position[1], $modules))
			{
				$output .= "<div id='$position[1]'>";
				
				foreach($modules[$position[1]] as $module)
				{
					$view = ModuleView::create($module);
					$output .= $view->drawView();
				}
				
				$output .= "</div>";				
			}
			
			$template = str_replace($position[0], $output, $template);
		}
		
		// Pull in the script and style dependencies defined by the components
		
		$script = Fakoli::getScripts() . $script;
		$styles = Fakoli::getStyles() . $styles;
		
		// Populate global variable references
		
		$vars = array();
		
		preg_match_all("/\{var:(.*?)\}/", $template, $vars, PREG_SET_ORDER);
		
		foreach($vars as $var)
		{		
			$expr = $var[0];
			$name = $var[1];	
			$template = str_replace($expr, $$name, $template);
		}
				
		$template = ComponentManager::fireEvent("PostProcessPage", $template);
		
		if (!$config['prettyURLs'])
		{
			$template = preg_replace("/(href|src|action)=(['\"])\\/?([\\w\\d_]+?)\\?/i", "$1=$2/page.php?identifier=$3&", $template);
			$template = preg_replace("/(href|src|action)=(['\"])\\/?([\\w\\d_]+?)[\"']/i", "$1=$2/page.php?identifier=$3$2", $template);
			$template = preg_replace("/go\\(([\"'])\\/?([\\w\\d_]+?)\\?/", "go($1/page.php?identifier=$2&", $template);
			$template = preg_replace("/go\\(([\"'])\\/?([\\w\\d_]+?)[\"']/", "go($1/page.php?identifier=$2$1", $template);
			
		}
		
		return $template;
	}
}

?>
