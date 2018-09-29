<?php
Fakoli::using("report", "report_manager");

if(!checkRole("admin,data"))
{
	Fakoli::end("Insufficient privileges to share a report.");
}

$report_id = checkNumeric($_GET["report_id"]);
$shared = checkNumeric($_GET["shared"]);

if ($report_id)
{
	$report = new CustomReport($report_id);
	$report->shared = $shared;
	$report->save();
}

Fakoli::end("OK");
?>