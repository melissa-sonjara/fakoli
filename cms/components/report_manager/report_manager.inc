<?php
/**************************************************************

 Copyright (c) 2007-2010 Sonjara, Inc

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

Fakoli::usingFeature("tree", "search_form", "data_view");

/**
 * ReportManager provides a generic mechanism for implementing user configurable reports based on DataItem classes.
 * 
 * @author andy
 *
 */
class ReportManager
{
	var $tables;
	var $join;
	var $report;
	
	function ReportManager()
	{
		$this->tables = array();
	}
	
	/**
	 * Add a table to the report.
	 * @param string $class the DataItem class representing the table in the datamodel
	 * @param string $title (optional) the title to present to the user in the configuration interface
	 */
	function table($class, $title = "")
	{
		$table = new ReportTable($class, $title);
		$this->tables[] = $table;
		return $table;
	}
	
	/**
	 * Draw the configuration/search form.
	 */
	function drawForm()
	{
		global $script;
		
		$tree = new TreeControl("custom_report");
		$tree->scroll = true;
		$tree->width = 310;
		$tree->height = 430;
		$tree->selectMode = "multi";
		$tree->onSelect = "ReportManager.updateFilters";
		
		foreach($this->tables as $table)
		{
			$tableNode = new TreeNode("table_{$table->class}", $table->getTitle(), $table->class, false, "tree_node_closed", "tree_node_open");
						
			foreach($table->columns as $column)
			{
				$columnNode = new TreeNode("column_".codify($column->title), $column->title, $column->title, false,  "plain_tree_closed", "plain_tree_open");
				$columnNode->leafStyle = "file_node_leaf";
				
				$tableNode->add($columnNode);
			}
			
			$tree->add($tableNode);
		}
		
		$script .= $tree->writeScript();

		
		echo "<form method='POST' action=''>";
		echo "<div style='float: left; width: 320px'>";
		echo "<strong>Select Report Fields</strong><br/><br/>";
		$tree->writeHTML();
		echo "</div><div style='float: left: width: 550px'>";
		echo "<strong>Filter By</strong><br/><br/>";
		
		foreach($this->tables as $table)
		{
			echo "<fieldset id='{$table->class}' style='display: none'><legend>".$table->getTitle()."</legend>\n";
			
			echo "<table ";
			if ($this->formCSS) echo "class='{$this->formCSS}'"; 
			echo ">\n";

			$table->form->renderSearchFields();

			echo "</table>\n";
			echo "</fieldset>";
		}
		
		echo "</div><div style='clear: left'><br/><input type='checkbox' name='__excel' value='custom_report'/> <strong>Output as Excel</strong><br/><br/><input type='submit' class='button' value='Generate Report'/></div></form>";
	}
	
	/**
	 * Retrieve the ReportTable object for the specified DataItem class.
	 * @param string $class the class identifying the ReportTable to be retrieved. 
	 */
	function getTable($class)
	{
		foreach($this->tables as $table)
		{
			if ($table->class == $class) return $table;
		}
		
		return null;
	}
	
	
	function generateReport()
	{
		global $script;
		
		$this->join = new InnerJoin();
		$this->join->unique = true;
		$constraint .= "";
		$first = true;
		
		foreach($this->tables as $table)
		{
			if ($_REQUEST["table_{$table->class}"])
			{
				$this->join->add($table->class);
				$constraint .= $table->getConstraint($first);
				$first = false;
			}
		}
		
		$results = $this->join->query($constraint);
		
		$report = new DataListView($results, "custom_report");
		
		foreach($this->tables as $table)
		{
			if ($_REQUEST["table_{$table->class}"])
			{
				foreach($table->columns as $column)
				{
					if ($_REQUEST["column_".codify($column->title)])
					{
						$report->column($column->title, $column->format, $column->sortable, $column->style, $column->type);
					}
				}
			}
		}
		
		$report->excelFile = "custom_report_".date("Y_m_d_Hi").".xls";
		
		$script .= $report->writeScript();
		
		$report->cssClass = "list small";
		$report->drawView();
	}
}

class ReportTable
{
	var $class;
	var $proto;
	var $fields;
	var $title;
	var $form;
	var $constraints;
	
	function ReportTable($class, $title = "")
	{
		$this->class = $class;
		$this->proto = new $class;
		$this->fields = array();
		$this->title = $title ? $title : $this->proto->prettifyClassName();
		$this->constraints = $constraints;
		
		$this->form = new SearchForm($this->proto);
	}

	function column($title, $format, $sortable = true, $style = "", $type = null)
	{
		$this->columns[] = new ReportColumn($title, $format, $sortable, $style, $type);
		return $this;
	}

	function getTitle()
	{
		return $this->title;
	}
	
	function searchFields()
	{
		$filter = new InclusionFilter();
		for($i = 0; $i < func_num_args(); ++$i)
		{
			list($field, $searchMode) = explode(":", func_get_arg($i));
			$filter->add($field);
			if ($searchMode)
			{
				$this->form->setMatchingMode($searchMode, $field);
			}
		}
		
		$this->proto->filter = $filter;
		return $this;
	}
	
	function additionalConstraints($constraint)
	{
		$this->constraints = $constraint;
	}
	
	function getConstraint($first)
	{
		$params = new SearchParameters($this->proto, $first);
		$params->fromArray($_REQUEST);
		$constraint = $params->generateConstraint($first);
		
		if ($this->constraints)
		{
			if ($constraint)
			{
				$constraint .= " AND $constraint";
			}
			else
			{
				$constraint .= ($first ? "WHERE " : " AND ").$this->constraints;
			}
		}
		
		return $constraint;
	}
}

class ReportColumn
{
	var $title;
	var $format;
	var $style;
	var $sortable;
	var $type;
	
	function ReportColumn($title, $format,  $sortable = true, $style = "", $type = null)
	{
		$this->title = $title;
		$this->format = $format;
		$this->sortable = $sortable;
		$this->style = $style;
		$this->type = $type;
	}
}

?>