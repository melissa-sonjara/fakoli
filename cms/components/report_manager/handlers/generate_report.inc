<?php

Fakoli::using("report");

$report_id = checkIdentifier($_REQUEST["report_id"]);
$target = checkIdentifier($_GET["target"]);

$mode = ($method == "POST") ? "post" : "";

// var_dump($_REQUEST);
// echo "<br><br>";

if ($report_id)
{
	$report = ReportManager::load($report_id, $mode);
}
else if ($target)
{
	$report = CustomReportManager::createManager($target);
	$report->fromRequest();
}
else
{
	Fakoli::end("Missing Report");
}

if ($report->title)
{
	$page->page_title = $report->title;
}

$report->generateReport(true);

?>