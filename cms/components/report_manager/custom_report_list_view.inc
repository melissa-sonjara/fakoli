<?php
Fakoli::usingFeature("paged_list");

class CustomReportListView
{
	var $title;
	var $reports;
	var $list;
	var $id; // report url
	var $resultsPage;
	
	function __construct($title, $id, $constraints, $pageSize)
	{
		$this->title = $title;
		
		// blank title means throw-away record presaved on generate page
		$this->reports = Query::create(CustomReport, "WHERE title != '' AND $constraints")->execute();
		
		$this->id = $id;
		$this->list = new PagedList($this->reports, "{$id}_list", array($this, formatReport));
		$this->list->pageSize = $pageSize;
		$this->list->emptyList = "<em>No reports found.</em>";
	}
	
	function formatReport($report)
	{
		global $user;
		
		$out = "<div class='report_title'>";
		if ($report->shared)
		{
			$out .= "<i class='fa-fw fas fa-share'></i>&nbsp;";
		}
		
		$out .= "{$report->title}</div><div class='report_description'>{$report->description}</div>";
		
		$out .= "<a class='button' href='custom_report_results?report_id={$report->report_id}'><i class='fa-fw far fa-file-alt'></i> Run Report</a>";
		
		if (checkRole("admin,data") || $report->user_id == $user->user_id)
		{
			$out .= "&nbsp;&nbsp;&nbsp;<a class='button' href='custom_report?report_id={$report->report_id}&edit=1'><i class='fa-fw fas fa-pencil-alt'></i> Edit</a>";
		}
		
		if (checkRole("admin,data"))
		{
			if ($report->shared)
			{
				$out .= "&nbsp;&nbsp;&nbsp;<a class='button' href='#' onclick='ReportManager.unshareReport({$report->report_id}); return false;'><i class='fa-fw far fa-share reverse'></i> Unshare</a>";
			}
			else
			{
				$out .= "&nbsp;&nbsp;&nbsp;<a class='button' href='#' onclick='ReportManager.shareReport({$report->report_id}); return false;'><i class='fa-fw far fa-share'></i> Share</a>";
			}
			
			$out .= "&nbsp;&nbsp;&nbsp;<a class='button' href='#' onclick='ReportManager.deleteReport({$report->report_id}); return false;'><i class='fa-fw far fa-trash-alt'></i> Delete</a>";
		}
		return $out;
	}
	
	function writeScript()
	{
		return $this->list->writeScript();
	}
	
	function drawView()
	{
		echo "<div id='$this->id' class='custom_report_list_panel'>";
		echo "<h3>$this->title</h3>";
		echo "<div class='report_list'>";
		$this->list->drawList();
		echo "</div></div>";
	}
}