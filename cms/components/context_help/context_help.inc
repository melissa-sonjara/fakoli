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

class ContextHelpSerializationHandler
{
	function ContextHelpSerializationHandler()
	{
	}

	function export()
	{
		return SerializationManager::serialize(ContextHelp, "ORDER BY help_id");
	}
	
	function import($doc, $tx)
	{
		SerializationManager::unserialize(ContextHelp, $doc, $tx);
	}	
}

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
		$cl = $obj->contextHelpProxy ? $obj->contextHelpProxy : get_class($obj);
		
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
			$links .= "<a class=\"context_link\" href=\"javascript:toggleContextHelp('help-{$field}');\" onmouseover=\"showContextHelp('help-{$field}');\" onmouseout=\"hideContextHelp('help-{$field}');\">(?)</a>&nbsp;";
			$text  .= "<div id=\"help-$field\" class=\"contexthelp\">\n";
			$text  .= "<p id=\"heading-$field\">$title Help</p>\n";
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
	
	static function upgradeComponent($version)
	{
		$mgr = new ContextHelpUpgradeManager();
		$mgr->upgrade($version);
	}		
	
	static function onInitialize()
	{
		global $auto_form_defaults;
		
		$auto_form_defaults["onFormatLabel"] = "addContextHelp";
	}
	
	static function registerSerializationHandler()
	{
		SerializationManager::registerHandler("context_help", "Context Help", new ContextHelpSerializationHandler());
		return true;
	}	
}

function addContextHelp($form, $field, $label)
{
	$contextHelpMgr = ContextHelpManager::getInstance();
	$label .= "&nbsp;".$contextHelpMgr->formatLinks($form, $field);
	
	return $label;
}
?>