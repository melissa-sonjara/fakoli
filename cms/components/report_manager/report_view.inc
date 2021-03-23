<?php
/**\addtogroup report_manager
 * @{
 */


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

Fakoli::usingFeature("data_view");


/*
 * Retrieves and displays custom reports that are in
 * the requested manager class.
 * 
 * @param manager_class - the class that extends ReportManager
 * and supplies the tables, fields and format templates for
 * the report (e.g., ProgramReportManager)
 * 
 * @param identifier - the identifier of that page that displays
 * the report form and generated results (e.g., custom_program_report).
 */
class ReportListView
{
	var $reports;
	var $table;
	var $identifier; // report url
	var $resultsPage;
	var $buttons_at_top = true;
	
	function ReportListView($manager_class, $identifier)
	{
		// blank title means throw-away record presaved on generate page
		$this->reports = Query::create(CustomReport, "WHERE manager_class=:manager_class AND title != '' ORDER BY title")
			->bind(":manager_class", $manager_class)
			->execute();
			
		$this->identifier = $identifier;
		$this->table = $this->buildTable($reports);	
	}
	
	function buildTable()
	{		
		// NOTE: "CustomReports" id is referenced in report_manager.js function reportSaved
		$table = new DataListView($this->reports, "CustomReports");
		$table->column("Title",  "<a href='{$this->identifier}?report_id={report_id}&edit=1'>{title}</a>", true)
			  ->column("Description", "{description}")
			  ->column("Actions", "<a class='button' href='{$this->identifier}?report_id={report_id}'>Run</a>&nbsp;<a class='button' href='{$this->identifier}?report_id={report_id}&edit=1'>Edit</a>");
			
		$table->pageSize=20;
		$table->emptyMessage = "There are no custom reports.";
		$table->filter=true;
		$table->sortable = true;

		return $table;
	}
	
	
	function writeScript()
	{
		return $this->table->writeScript();	
	}
	
	function drawView()
	{
		if($this->buttons_at_top)
		{
			echo "<p><a class='button' href='{$this->identifier}'>Create a New Report</a></p>\n";
		}
		
		$this->table->drawView();
		
		echo "<p><a class='button' href='{$this->identifier}'>Create a New Report</a></p>\n";
	}
}
/**@}*/?>