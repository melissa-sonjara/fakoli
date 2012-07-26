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

require_once realpath(dirname(__FILE__)."/../../../../framework/data_item.inc");

class ComponentPage extends DataItem
{
	// Fields
	var $fields = array("component_page_id"	=>	Number,
						"identifier"		=>	String,
						"server_path"		=>	String,
						"component"			=>	String,
						"page_title"		=>	String,
						"template"			=>	String,
						"role"				=>	String,
						"enabled"			=>	Boolean,
						"scanned"			=>	Boolean,
						"site_id"			=>	Number
	);
	
	// Relations
	var $relations = array(
							"Component"	=>	Component,
							"Site"		=>	Site
						);
	
	function Component()
	{
		return $this->getRelated(Component);
	}
	
	function Site()
	{
		return $this->getRelated(Site);
	}
	
	function ComponentPageModuleXrefs($constraint = "ORDER BY position, sort_order")
	{
		return $this->getRelatedList(ComponentPageModuleXref, "", $constraint);
	}
	
	function Modules($constraint = "ORDER BY position, sort_order")
	{
		return $this->crossReference(Module, ComponentPageModuleXref, $constraint);
	}
	
	
	function getModulesByPosition($constraint = "ORDER BY position, sort_order")
	{
		global $section;
		
		$join = new InnerJoin();
		$join->add(Module);
		$join->add(ComponentPageModuleXref);
		
		$result = $join->groupedQuery("WHERE component_page_id={$this->component_page_id} $constraint", "ComponentPageModuleXref.position");
		$modules = extractGroupedJoinResults(Module, $result);
		
		// If no modules have been set, use the section defaults
		if (count($modules) == 0 && $section)
		{
			$modules = $section->getModulesByPosition($constraint);
		}
		
		// Merge in global modules
		$globals = groupedQuery(Module, "WHERE global=1", "global_position");
		foreach($globals as $position => $modulesAtPosition)
		{
			if (array_key_exists($position, $modules))
			{
				$modules[$position] = removeDuplicates(array_merge($modulesAtPosition, $modules[$position]));
			}
			else
			{
				$modules[$position] = $modulesAtPosition;
			}
		}
		
		return $modules;
	}
	
	/**
	 * Loads the template associated with this page.
	 * @return string the contents of the template for this page.
	 */
	function getTemplate()
	{
		global $config;
		global $section;
		
		$style = $_REQUEST["_style"];
		$site = $this->Site();
		if(!$site) Site::getSite();
		
		$templateFile = "";
		
		switch($style)
		{
		case 'print':
			$templateFile = $site->print_template;
			break;

		case 'popup':
			$templateFile = $site->popup_template;
			break;
			
		case 'mobile':
			$templateFile = $site->mobile_template;
			
		case 'nude':
			return "{description}";
		}

		if ($section)
		{
			if (!$templateFile) $templateFile = $section->getTemplateFile($this->identifier);
		}
				
		if (!$templateFile) $templateFile = $this->template;
		if (!$templateFile) $templateFile = $site->default_template;
		
		$templateFile = ComponentManager::fireEvent("OverrideTemplate", $templateFile);
		
		$template = file_get_contents("{$config['homedir']}/templates/{$templateFile}");

		return $template;
	}
		
	
	/**
	 * Retrieves an array of the position names defined in the template for this page.
	 * @return array the positions defined in the template (sorted alphabetically)
	 */
	function getPositions()
	{
		$template = $this->getTemplate();
		
		$positionMatches = array();
		
		preg_match_all("/\{position:(.*?)\}/", $template, $positionMatches, PREG_PATTERN_ORDER);
		
		$positions = $positionMatches[1];		
		sort($positions);
		return $positions;
	}
	
	static function findByIdentifier($identifier, $constraint = "")
	{
		$constraint = preg_replace("/^\s*WHERE\s+/i", "AND ", $constraint);
		
		$match = Query::create(ComponentPage, "WHERE identifier=:id $constraint")
					  ->bind(":id", $identifier)
					  ->executeSingle();

		return $match;
	}
	
	function ComponentPage()
	{
		$this->primary_key = "component_page_id";
		$this->table = "component_page";
		
		$this->default_format = "{identifier}";
		
		$this->DataItem(func_get_args());
	}
}



class ComponentPageModuleXref extends DataItem
{
	// Fields
	var $fields = array("join_id"			=>	Number,
						"component_page_id"	=>	Number,
						"module_id"			=>	Number,
						"position"			=>	String,
						"sort_order"		=>	Number);
	
	// Relations
	
	var $relations = array(	"Page"			=>	ComponentPage,
							"Module"		=>	Module);
	
	function Page()
	{
		return $this->getRelated(Page);
	}
	
	function Module()
	{
		return $this->getRelated(Module);
	}
	
	function ComponentPageModuleXref()
	{
		$this->table = "component_page_module_xref";
		$this->primary_key = "join_id";
		
		$this->DataItem(func_get_args());
	}
}