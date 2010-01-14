<?php
require_once realpath(dirname(__FILE__))."/../../datamodel/page.inc";
require_once realpath(dirname(__FILE__))."/menu_view.inc";
require_once realpath(dirname(__FILE__))."/../../datamodel/menus.inc";
require_once realpath(dirname(__FILE__))."/module_view.inc";

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
		$template = $this->page->getTemplate();
		
		// If the page has a PHP code include, execute it and insert the contents
		
		if ($this->page->php_code_file)
		{
			ob_start();
			include realpath(dirname(__FILE__))."/../modules/{$this->page->php_code_file}";
			$output = ob_get_contents();
			ob_end_clean();
			
			$this->page->description .= $output;
		}
		
		// Populate template fields from the Page object.
		
		foreach($this->page->fields as $field => $type)
		{
			$template = str_replace("{".$field."}", $this->page->$field, $template);
		}		
		
		// Populate the template with the appropriate menu.
		
		$menu = new MenuView(new Menus());
		
		ob_start();
		
		$menu->drawView($this->page->template);
		
		$output = ob_get_contents();
		
		ob_end_clean();
		
		$template = str_replace('{menu}', $output, $template);
		
		$positions = array();
		
		preg_match_all("/\{position:(.*?)\}/", $template, $positions, PREG_SET_ORDER);
		
		// Populate the template with associated modules for the page at the correct positions.
		
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
		
		// Populate global variable references
		
		$vars = array();
		
		preg_match_all("/\{var:(.*?)\}/", $template, $vars, PREG_SET_ORDER);
		
		foreach($vars as $var)
		{		
			$expr = $var[0];
			$name = $var[1];	
			$template = str_replace($expr, $$name, $template);
		}
		
		return $template;
	}
}

?>
