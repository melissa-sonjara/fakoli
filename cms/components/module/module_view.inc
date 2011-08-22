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

/**************************************************************

Title: module_view.php

Description: To view the module.

author Reshma Joshi for Sonjara, Inc.

date: 11/09

***************************************************************/

Fakoli::using("article", "document");

abstract class ModuleView
{
	var $module;
	
	function ModuleView($module)
	{
		$this->module = $module;
	}

	static function create($module)
	{
		switch($module->content_type)
		{
		case "Menu":
			return new MenuModuleView($module);				
		case "Code":
			return new CodeModuleView($module);
		case "HTML":
			return new HTMLModuleView($module);
		default:
			return new QueryModuleView($module);
		}
	}
	
	abstract function drawView();
}

class QueryModuleView extends ModuleView
{
	function QueryModuleView($module)
	{
		$this->ModuleView($module);
	}

	function drawView($cssClass = "")
	{
		$cssClass = trim("module {$this->module->css_class} $cssClass");
		
		$title = $this->module->getTitle();
		
		$className= $this->module->getContent_Type();
        
		$constraint = $this->module->getConstraint();
		if (!$constraint) { 
			$constraint = "1=1";
		}
		else if($constraint == "site_user_id") {
			global $user;
            $constraint = " site_user_id='$user->site_user_id' ";
	    }
		$limit=  $this->module->getNum_Items();
		
		if ($limit) $limit = "LIMIT $limit";
		
		$orderby= $this->module->getOrd_By();
				
		$results = query($className, "WHERE $constraint ORDER BY $orderby DESC $limit");

		ob_start();
		
    	eval($this->module->template);
    	
    	$template = trim(ob_get_contents());
    	
    	ob_end_clean();
    			
    	return $template ? "<div class='$cssClass'>".$template."</div>" : "";
	}
}

class CodeModuleView extends ModuleView
{
	function CodeModuleView($module)
	{
		$this->ModuleView($module);
	}
	
	function drawView($cssClass = "")
	{
		$cssClass = trim("module {$this->module->css_class} $cssClass");
		
		global $config;
		global $script;
		global $styles;
		global $dialogs;
		global $page;
		global $site;
		global $user;
		
		if ($this->module->php_code_file)
		{
			ob_start();
			echo "<div class='$cssClass'>";
			
			$codePath = explode(",", $this->module->php_code_file);
			if (count($codePath) == 2)
			{
				$name = $codePath[0];
				$php_code_file = $codePath[1];
				$components = Fakoli::getComponents();
				if (array_key_exists($name, $components))
				{
					$path = $components[$name]->component_path;
					include $path."/{$php_code_file}";
				}
				else
				{
					throw new FakoliException("Unknown component");
				}
			}
			else
			{
				include $config['homedir']."/{$this->module->php_code_file}";
			}
    		echo "</div>";
    		$output = ob_get_contents();
 			ob_end_clean();
		}	

		return $output;
	}
}

class HTMLModuleView extends ModuleView
{
	function HTMLModuleView($module)
	{
		$this->ModuleView($module);
	}
	
	function drawView($cssClass = "")
	{
		$cssClass = trim("module {$this->module->css_class} $cssClass");
		return "<div class='$cssClass'>{$this->module->template}</div>";
	}
}

class MenuModuleView extends ModuleView
{
	function MenuModuleView($module)
	{
		$this->ModuleView($module);
	}
	
	function drawView($cssClass = "")
	{
		$cssClass = trim("module {$this->module->css_class} $cssClass");
		$menu = $this->module->Menu();
		
		$params = explode(",", $this->module->menu_parameters);

		$qs = "";
		$x = "?";
		
		foreach($params as $param)
		{
			if ($_GET[$param]) 
			{
				$qs .= $x . $param . "=" . $_GET[$param];
				$x = "&";
			}			
		}
		
		trace("QUERYSTRING: $qs", 3);
		
		$menuView = new MenuView($menu);
		$menuView->queryString = $qs;
		return "<div class='$cssClass'>".$menuView->drawView()."</div>";
	}
}
?>