<?php

Fakoli::using("report");

$report_id = $_GET["report_id"];
$edit = checkNumeric($_GET["edit"]);
$mode = $_POST["custom_report_mode"];
$target = checkIdentifier($_GET["target"]);

if($report_id)
{
	$customReport = ReportManager::load($report_id, $mode);
	
	$page->page_title = $customReport->title;
	
	if ($edit)
	{
		if ($mode == "save")
		{
			$customReport->save();
		}
		
		$customReport->drawForm();
	}
	else
	{
		$customReport->generateReport();
	}
}
else
{
	if (!$target)
	{
		throw new FakoliException("No target specified");
	}
	
	$customReport = CustomReportManager::createManager($target);
	
	$page->page_title = "Create a New Report";
	
	if ($method == "POST" || isset($_REQUEST["__excel"]))
	{
		if ($mode == "save")
		{
			$customReport->save();
			redirect("custom_report?report_id={$customReport->report_id}&edit=1");
		}
		else
		{
			$customReport->generateReport();
		}
	}
	else
	{
		$customReport->drawForm();
	}
}
?>
