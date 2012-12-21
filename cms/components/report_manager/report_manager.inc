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
 * To use ReportManager:
 * 
 * 1) create a component "report" in your application's components
 * and create its manifest.inc file
 * 
 * 2) create folder "pages" with a file for each type of
 * report needed in the application with names such as 
 * "custom_xx_report" (e.g., custom_program_report)
 * 
 * 3) The page or pages should contain this code:  
 * 
Fakoli::using("report");

$report = new XXReportManager();

if ($method == "POST" || isset($_REQUEST["__excel"]))
{
	$report->generateReport();
}
else
{
	$report->drawForm();
}

 * 4) On the top level in the report component,
 * add a php file xx_report_manager.inc with the code:
 * 
class XXReportManager extends ReportManager
{
	function XXReportManager()
	{
		$this->ReportManager();

		$this->table(MyDataItemClass, "MyTitle")			
			->column("My Field Label", "{my_field}", true, "")
			...
			;
	}
}

 * 
 * the same way you would create a DataListView
 * Before closing the column list with the semicolon,
 * you can also add search fields with the search mode
 * (equal if not provided):
 * 
 * ->searchFields("contact_id", "name:like")
 * 
 * For fields that have values in a related table, create
 * a RelatedItemSelectFieldRenderer for the search filter
 * Example:	
 * 
 * new RelatedItemSelectFieldRenderer($this->getTable(Program)->form, "affiliation_id", "Primary Program Type", Affiliation, "ORDER BY sort_order, affiliation", "affiliation", "affiliation_id");
 * 
 * If the field has a set of options defined in the
 * DataItem class, create SelectFieldRenderer.
 * Example:
 * 
 * new SelectFieldRenderer($this->getTable(Program)->form, "residency_requirement", "Residency Requirement", Program::$residencyTypes);	
 * 
 * If you need to search on a field that is not in the table
 * but is instead in an xref table, such as the obj is linked
 * to a set of states through a item_state_xref table, then
 * create the RelatedItemSelectFieldRenderer but set an 
 * additional or custom callback parameter handler:
 * 
 * new RelatedItemSelectFieldRenderer($this->getTable(Program)->form, "state", "State", State, "ORDER BY name", "name", "state_id");	
 * 
 * $this->getTable(Program)->params->setParam("state", "equal", $_REQUEST["state:equal"]);
 * $this->getTable(Program)->params->setHandler("state", array(ProgramReportManager, getStateConstraint));
 * 
 * @author andy
 *
 */
class ReportManager
{
	var $report_id;			///< ID of the saved report as persisted in the database
	var $title;				///< Title of the report
	var $description;		///< Longer description of the report
	var $resultsPage = "";	///< The page used to display the search results
	var $editorPage = "";	///< The page used to display the report editor
	var $target;			///< The target window for the form results
	var $columnOrder;		///< Override the default column order
	var $tables;
	var $join;
	var $report;
	var $request;
	var $interstitialMessage = "";	///< Optional interstitial message to display while report is being generated
	var $limit;						///< Optional limit to number of records retrieved, depending on acceptable load times for results
	
	/**
	 * Creates a new ReportManager instance.
	 */
	function ReportManager()
	{
		$this->selected = null;
		
		$this->report_id = 0;
		$this->tables = array();
		$this->request = $_REQUEST; // For config serialization
		if ($this->request["custom_report_mode"] == "save")
		{
			$this->title = $this->request["custom_report_title"];
			$this->description = $this->request["custom_report_description"];
		}		
	}
	
	/**
	 * Tidy up the request data and limit the fields returned when serializing a ReportManager.
	 * 
	 * @return array
	 */
	function __sleep()
	{
		$this->request = array_remove_keys($this->request, "custom_report_mode", "PHPSESSID", "__utma", "__utmb", "__utmc", "__utmz");		
		return array('report_id', 'title', 'description', 'request');	
	}
	
	/**
	 * Add a table to the report.
	 * @param string $class the DataItem class representing the table in the datamodel
	 * @param string $title (optional) the title to present to the user in the configuration interface
	 */
	function table($class, $title = "", $useOutputFilter = false)
	{
		$table = new ReportTable($class, $title, $this->request, $useOutputFilter);
		$this->tables[] = $table;
		return $table;
	}
	
	/**
	 * Draw the configuration/search form.
	 */
	function drawForm()
	{
		global $script;
		
		$tableNavigator = new ReportTableNavigator(&$this->tables);
		$filterNavigator = new ReportFilterNavigator(&$this->tables);
		
		$script .= $tableNavigator->writeScript();
		$script .= $filterNavigator->writeScript();
		
		$action = $this->resultsPage ? $this->resultsPage : Fakoli::scriptName();
		$target = $this->target ? " target='{$this->target}'" : "";
		
		$interstitial = $this->interstitialMessage ? " onsubmit='interstitial(\"".jsSafe($this->interstitialMessage).".\"); return true;'" : "";
		
		echo "<form method='POST' action='$action?report_id={$_GET['report_id']}'$target id='custom_report'$interstitial>";
		echo "<input type='hidden' name='report_id' value='{$this->report_id}'/>";
		echo "<input type='hidden' name='custom_report_mode' value='search' id='custom_report_mode'/>";
		echo "<input type='hidden' name='custom_report_title' value='' id='custom_report_title'/>";
		echo "<input type='hidden' name='custom_report_description' value='' id='custom_report_description'/>";
		$tableNavigator->drawView();
		$filterNavigator->drawView();
		
		
		$incomplete = ($this->request["__include_incomplete"] || $this->report_id == 0) ? " checked='checked'" : "";
		$excel = $this->request["__excel"] ? " checked='checked'": "";
		
		echo "<div style='clear: left'><br/><input type='checkbox' name='__include_incomplete' value='1'$incomplete/> <strong>Include Incomplete Records in Results</strong>";
		//echo "<br/><input type='checkbox' name='__excel' value='custom_report'$excel/> <strong>Output as Excel</strong>";
		echo "<br/><br/><input type='submit' class='button' value='Generate Report'/>";
		//echo "&nbsp;&nbsp;<button class='button' onclick='ReportManager.saveReport($this->report_id); return false;'>Save Report</button>";
		echo "</div></form>";
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
	
	/*
	 * JDG 2/2012 avoid crash when user does not
	 * check any tables/columns - by default
	 * check first table and all its columns.
	 * If table is checked w/o any columns checked
	 * then select all columns in that table. 
	*/
	function populateSelected()
	{
		if ($this->request["custom_report_title"]) $this->title = $this->request["custom_report_title"];
		if ($this->request["custom_report_description"]) $this->description = $this->request["custom_report_description"];
		
		foreach($this->tables as $table)
		{
			if ($this->request["table_{$table->class}"])
			{
				$this->setSelected($table);
			}
			$table->load($this->request);
		}
		
		if(count($this->selected) == 0)
		{
			$this->setSelected($this->tables[0], false);
		}
		
	}
	
	/**
	 * Set the given table as selected
	 * 
	 * @param ReportTable $table the table to select
	 * @param boolean $checkRequest the check request flag
	 */
	function setSelected($table, $checkRequest = true)
	{
		$table->selected = true;
		$this->selected["table_{$table->class}"] = true;
		//$this->constraint .= $table->getConstraint($first, $this->request);

		$colCount = 0;
		foreach($table->columns as $column)
		{
			$col = "column_".codify($column->title);
			if ($this->request[$col] || !$checkRequest)
			{
				$column->selected = true;
				$this->selected[$col] = true;
				$colCount ++;			
			}
		}	
		
		if($colCount == 0)
			$this->setSelected($table, false);
	}
	
	/**
	 * Generate the search constraint for the current report configuration.
	 * 
	 * @return string the SQL constraint for the search 
	 */
	function generateConstraint()
	{
		foreach($this->tables as $table)
		if ($this->selected["table_{$table->class}"])
		{
			$this->constraint .= $table->getConstraint($first, $this->request);
		}
		
		if($this->limit && is_numeric($this->limit))
		{
			$this->constraint .= "LIMIT {$this->limit}";
		}
	}
	
	function generateReport($inlineScript = false)
	{
		$t1 = microtime(true);
		
		$this->preSaveReport();
		
		global $script;
		
		if (!isset($this->selected))
		{
			$this->populateSelected();
		}
		
		$this->generateConstraint();
		
		if ($_REQUEST["custom_report_mode"] == "save")
		{
			$this->save();
			return;
		}
		
		$joinClass = ($this->request['__include_incomplete'] == "1") ? LeftOuterJoin : InnerJoin;
		
		$this->join = new $joinClass;
		$this->join->unique = true;
		$constraint .= "";
		$first = true;
		
		foreach($this->tables as $table)
		{
			if ($this->selected["table_{$table->class}"])
			{
				$table->createJoin($this);
				
				
				$first = false;
			}
		}
		
		$results = $this->join->iteratedQuery($this->constraint);
		
		$report = new DataListView($results, "custom_report_results");
		
		foreach($this->tables as $table)
		{
			if ($this->selected["table_{$table->class}"])
			{
				$table->createColumns($this, $report);
			}
		}
		
		$report->dragColumnReorder = true;
		$report->columnReorderCallback = "ReportManager.setColumnOrder";
		
		if ($this->columnOrder)
		{
			$report->setColumnOrder(explode("|",$this->columnOrder));
		}
		
		$reportTitle = ($this->title) ?  codify($this->title)."_".date("Y_m_d_Hi") : "custom_report_".date("Y_m_d_Hi");
		//if(isset($this->request["__excel"]))
		//{
			$report->excelFile = $reportTitle . ".xls";
		//}
		/* to do - pdf output 
		else
		{
			$identifier = $this->request["identifier"];
			$pdf = new PDF("/$identifier", false, true, $reportTitle . ".pdf");
			$pdf->generate();
		}
		*/
		if ($inlineScript)
		{
			echo $report->writeScript();
		}
		else
		{
			$script .= $report->writeScript();
		}
				
		$report->cssClass = "list small";
		$report->hideExcelIcon = true;
		
		$excel = $report->getExcelLink();
		
		if($this->limit && count($results))
		{
			echo "<p class='report_limit'>Results are limited to the first {$this->limit} records.</p>\n";
		}
		
		$this->writeResultsFormButtons($excel);
		
		$report->drawView();
		
		if (count($results))
		{
			$this->writeResultsFormButtons($excel);
		}
		
		$t2 = microtime(true);
		$elapsed = ($t2 - $t1);
		echo "<p><em>Report generated in ".number_format($elapsed, 2)." seconds</em></p>";
	}
	
	function isColumnSelected($column)
	{
		return $this->selected["column_".codify($column->title)];
	}
		
	function writeResultsFormButtons($excel)
	{
		$url = $this->editorPage;
		echo "<p><a class='button' href='#' onclick='go(\"{$url}?report_id={$this->report_id}&edit=1\"); return false;'>&laquo; Edit Report Settings</a></p>";
		echo "<p><a class='button' href='#' onclick='ReportManager.updateReport(\"{$this->report_id}\"); return false;'>Save Report</a>\n";
		echo "&nbsp;&nbsp;<a class='button' href='#' onclick='ReportManager.exportToExcel(\"$excel\"); return false;'>Export to Excel</a>";		
		echo "\n&nbsp;&nbsp;<a class='button' href='$url'>Create a New Report</a></p>\n";
	}
	
	/**
	 * Alternative implementation - presave version of
	 * report is stored in session with a 
	 */
	function preSaveReport()
	{		
		global $user;
		
		if (!$this->report_id)
		{
			$sleeper = serialize($this);
			$mgr = new UserManager();
			
			$this->report_id = "tmp-" .sha1($user->get($mgr->getUsernameField()) . $sleeper . now());
			$_SESSION[$this->report_id] = $sleeper;
			$_SESSION[$this->report_id . "-class"] = get_class($this);				
		}	
	}
		
	function save()
	{
		global $user;
		
		$report = new CustomReport();
		$report->report_id = $this->report_id;
		$report->title = $this->title;
		$report->description = $this->description;
		$report->user_id = $user->user_id;
		$report->configuration = serialize($this);
		$report->manager_class = get_class($this);
		$report->column_order = $this->columnOrder;
		
		$report->save();
		
		$this->report_id = $report->report_id;
	}
	
	static function load($report_id, $mode = "")
	{
		if (is_numeric($report_id))
		{
			$report = new CustomReport($report_id);
	
			$proto = unserialize($report->configuration);
			$cl = get_class($proto);
	
			if ($mode != "save")
			{
				//$_REQUEST = $proto->request;
			}
			
			$instance = new $cl;
			$instance->request = ($mode == "save") ? $_REQUEST : $proto->request;
			
			$instance->populateSelected();
			$instance->columnOrder = $report->column_order;
			$instance->report_id = $report_id;
			return $instance;
		}
		else if (startsWith($report_id, "tmp-"))
		{
			$proto = unserialize($_SESSION[$report_id]);
			$cl = get_class($proto);
			$instance = new $cl;
			$instance->request = $proto->request;
			$instance->populateSelected();
			$instance->columnOrder = $report->column_order;
			$instance->report_id = 0;
			return $instance;
		}
		else
		{
			throw new FakoliException("Invalid report id");
		}
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
	var $params;
	var $help;
	var $excludedKeys;
	var $columns = array();
	var $footerColumns = array();
	var $totalCallbacks = array();
	var $outputFilter = null;
	
	function ReportTable($class, $title = "", $request = null, $outputFilter = false)
	{
		$this->class = $class;
		$this->proto = new $class;
		
		if ($outputFilter) 
		{
			$this->outputFilter = new InclusionFilter();
			$this->outputFilter->add($this->proto->getPrimaryKey());
		}
		
		$this->fields = array();
		$this->title = $title ? $title : $this->proto->prettifyClassName();
		$this->constraints = $constraints;
		
		if ($request) $this->proto->fromDataSet($request);
		
		$this->form = new SearchForm($this->proto);
		$this->form->id = "custom_report";
		// JDG 2/2012 - set params here to allow additional parameters
		// to be set by the calling class
		$this->params = new SearchParameters($this->proto);
		
		$this->excludedKeys = array();
		$this->help = "";
	}

	// Control Serialization ----
	
	function __sleep()
	{
		return array('class', 'fields', 'title', 'constraints', 'params', 'excludedKeys', 'columns', 'footerColumns', 'totalCallbacks');	
	}
	
	function __wakeup()
	{
		$this->proto = new $this->class;
		$this->form = new SearchForm($this->proto);
	}
	
	function load($request)
	{		
		if ($request) 
		{
			$this->proto->fromDataSet($request);
			$this->params->fromArray($request);
			$this->form->params->fromArray($request);
		}
	}
	
	// ----
	
	function column($title, $format, $sortable = true, $style = "", $type = null, $outputFields = null)
	{
		$this->columns[] = new ReportColumn($title, $format, $sortable, $style, $type, $outputFields);
			
		return $this;
	}

	function excludeKeyFromJoin($key)
	{
		$this->excludedKeys[] = $key;
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
			// JDG 2/2012 - putting func_get_arg inside the explode crashes for me if not
			$fieldMode = func_get_arg($i);
			list($field, $searchMode) = explode(":", $fieldMode);
			$filter->add($field);
			if ($searchMode)
			{
				$this->form->setMatchingMode($searchMode, $field);
			}
		}
		
		$this->proto->filter = $filter;
		return $this;
	}
	
	function favoriteColumns()
	{
		$args = func_get_args();
		$favorites = array_combine($args, $args);
		foreach($this->columns as $column)
		{
			if (array_key_exists($column->title, $favorites))
			{
				$column->favorite = true;
			}
		}	
		
		return $this;
	}
	
	function additionalConstraints($constraint)
	{
		$this->constraints = $constraint;
		
		return $this;
	}
	
	function getConstraint($first, $request)
	{
		$this->params->fromArray($request);
		$constraint = $this->params->generateConstraint($first);
		
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
	
	/**
	 * Adds a text column to the table footer.
	 * @param $title the title of the columnn under which the footer value should be displayed
	 * @param $text the text to display
	 * @param $style any CSS classes or inline styles to apply
	 * @return ReportManager returns the current instance, allowing function call chaining for the column definitions.
	 */
	function footerText($text = "", $style = "")
	{
		$column = new ReportFooterColumn($title, $text, "", $style);
		$this->footerColumns[$title] = $column;
		
		return $column;
	}

	/**
	 * Adds a value column to the table footer.
	 * @param $title the title of the columnn under which the footer value should be displayed
	 * @param $callback Callback function or object method used to calculate the value for this column.
	 * @param $style any CSS classes or inline styles to apply
	 * @return ReportManager returns the current instance, allowing function call chaining for the column definitions.
	 */
	function footerValue($title, $callback, $style = "")
	{
		$column = new FooterValueColumn($callback, $style);
		$this->footerColumns[$title] = $column;
		
		return $column;
	}
	
	/**
	 * Sums the total for a column
	 * @param $title the title of the columnn under which the footer value should be displayed
	 * @param $field the field to total in the format "{field}"
	 * @param $style any CSS classes or inline styles to apply
	 * @return DataListView returns the current instance, allowing function call chaining for the column definitions.
	 */
	function footerTotal($title, $field, $style = "text-align: right")
	{
		$column = new FooterTotalColumn($field, $template = "", $style);
		$this->footerColumns[$title] = $column;
		
		return $column;
	}
	
	function createJoin($mgr)
	{
		$mgr->join->add($this->class);
		$this->filterOutput($mgr, $this->class);
		
		foreach($this->excludedKeys as $excluded)
		{
			$mgr->join->excludeKeyFromJoin($this->class, $excluded);
		}
	}

	function filterOutput($mgr, $class)
	{
		if ($this->outputFilter)
		{
			foreach($this->columns as $column)
			{
				if ($mgr->isColumnSelected($column) && is_array($column->outputFields))
				{
					foreach($column->outputFields as $field)
					{
						$this->outputFilter->add($field);
					}
				}
			}
		
			$mgr->join->setFilter($class, $this->outputFilter);
		}
	}
		
	function createColumns($mgr, $report)
	{
		foreach($this->columns as $column)
		{
			if ($mgr->isColumnSelected($column))
			{
				$report->column($column->title, $column->format, $column->sortable, $column->style, $column->type);
				
				if(array_key_exists($column->title, $this->footerColumns))
				{
					$footer = $this->footerColumns[$column->title];
					$report->footerColumns[] = $footer;
					if(get_class($footer) == FooterTotalColumn)
					{
						$report->totalCallbacks[] = array($footer, onStartRow);
					}
				}
				else 
				{
					$report->footerColumns[] = new FooterTextColumn("&nbsp;");
				}
			}
		}		
	}
}

class ReportColumn
{
	var $title;
	var $format;
	var $style;
	var $sortable;
	var $type;
	var $outputFields;
	
	function ReportColumn($title, $format,  $sortable = true, $style = "", $type = null, $outputFields = null)
	{
		$this->title = $title;
		$this->format = $format;
		$this->sortable = $sortable;
		$this->style = $style;
		$this->type = $type;
		$this->outputFields = $outputFields;
	}
}

?>