<?php
require_once $config['homedir']."/datamodel/context_help.inc";

class ContextHelpManager
{
	static $_instance;

	var $classes;
	var $nonmatches;
	
	static function getInstance()
	{
		if (!ContextHelpManager::$_instance)
		{
			ContextHelpManager::$_instance = new ContextHelpManager();
		}

		return ContextHelpManager::$_instance;
	}

	
	function ContextHelpManager()
	{
		$this->classes = array();
		$this->nonmatches = array();	
	}
	
	function getHelp($form, $field)
	{
		$obj = $form->data;
		$records = $this->getContextHelpRecords($obj);
		if (!$records) return null;
		
		if (array_key_exists($field, $records))
		{
			return $records[$field]->help;
		}
		
		return null;
	}
	
	function getCriteria($form, $field)
	{
		$obj = $form->data;
		$records = $this->getContextHelpRecords($obj);
		if (!$records) return null;
		
		if (array_key_exists($field, $records))
		{
			return $records[$field]->criteria;
		}
		
		return null;		
	}
		
	function getTitle($form, $field)
	{
		$title = null;
		$obj = $form->data;
		$records = $this->getContextHelpRecords($obj);
		if (!$records) return null;
		
		if (array_key_exists($field, $records))
		{
			$title = $records[$field]->title;
		}
		
		if (!$title)
		{
			$title = $form->prettifyFieldName($field);
		}
		
		return $title;		
	}
	
	function getContextHelpRecords($obj)
	{
		$cl = get_class($obj);
		
		if ($this->nonmatches[$cl]) return null;
		
		if ($this->classes[$cl])
		{
			return $this->classes[$cl];
		}
		
		$records = indexedQuery(ContextHelp, "WHERE class_name='$cl'", "field");
		if ($records)
		{
			$this->classes[$cl] = $records;
		}
		else
		{
			$this->nonmatches[$cl] = true;
		}
		return $records;
	}
	
	function formatLinks($form, $field)
	{
		$help = $this->getHelp($form, $field);
		$criteria = $this->getCriteria($form, $field);
		$title = $this->getTitle($form, $field);
		
		$links = "";
		$text = "";
				
		if ($help)
		{
			$links .= "<a class=\"context_link\" href=\"javascript:toggleContextHelp('help-{$field}');\" onmouseover=\"showContextHelp('help-{$field}');\" onmouseout=\"hideContextHelp('help-{$field}');\">(help)</a>&nbsp;";
			$text  .= "<div id=\"help-$field\" class=\"contexthelp\">\n";
			$text  .= "<h2 id=\"heading-$field\">$title Help</h2>\n";
			$text  .= $help;
			$text  .= "</div>\n";
		}
		
		if ($criteria)
		{
			$links .= "<a class=\"context_link\" href=\"javascript:toggleContextHelp('criteria-{$field}');\" onmouseover=\"showContextHelp('criteria-{$field}');\" onmouseout=\"hideContextHelp('criteria-{$field}');\">(criteria)</a>";
			$text  .= "<div id=\"criteria-$field\" class=\"contexthelp\">\n";
			$text  .= "<h2 id=\"heading-$field\">$title Criteria</h2>\n";
			$text  .= $criteria;
			$text  .= "</div>\n";
		}
		
		return "{$links}\n{$text}";
	}
}

function addContextHelp($form, $field, $label)
{
	$contextHelpMgr = ContextHelpManager::getInstance();
	$label .= "&nbsp;".$contextHelpMgr->formatLinks($form, $field);
	
	return $label;
}
?>