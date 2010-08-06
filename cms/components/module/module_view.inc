<?php

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
		
		echo "<div class='$cssClass'>";
		
    	eval($this->module->template);
    	
    	echo "</div>";
    	
    	$template = ob_get_contents();
    	
    	ob_end_clean();
    	
    	return $template;
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
		
		if ($this->module->php_code_file)
		{
			ob_start();
			echo "<div class='$cssClass'>";
			include $config['homedir']."/{$this->module->php_code_file}";
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