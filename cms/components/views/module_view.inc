<?php

/**************************************************************

Title: module_view.php

Description: To view the module.

author Reshma Joshi for Sonjara, Inc.

date: 11/09

***************************************************************/

require_once realpath(dirname(__FILE__))."/../../datamodel/module.inc";
require_once realpath(dirname(__FILE__))."/../../datamodel/news_item.inc";
require_once realpath(dirname(__FILE__))."/../../datamodel/publication.inc";

abstract class ModuleView
{
	var $module;
	
	function ModuleView($module)
	{
		$this->module = $module;
		//linkItemsInHTMLFields($this->module);
	}

	static function create($module)
	{
		switch($module->content_type)
		{
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

	function drawView()
	{
		
		$title = $this->module->getTitle();
		
		$className= $this->module->getContent_Type();

		$constraint = $this->module->getConstraint();
		if (!$constraint) $constraint = "1=1";

		$limit=  $this->module->getNum_Items();

		if ($limit) $limit = "LIMIT $limit";
		
		$orderby= $this->module->getOrd_By();
		
		$results = query($className, "WHERE $constraint ORDER BY $orderby DESC $limit");

		ob_start();
		
		echo "<div class='module'>";
		
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
	
	function drawView()
	{
		if ($this->module->php_code_file)
		{
			ob_start();
			echo "<div class='module'>";
			include realpath(dirname(__FILE__))."/../modules/{$this->module->php_code_file}";
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
	
	function drawView()
	{
		return "<div class='module'>{$this->module->template}</div>";
	}
}
?>