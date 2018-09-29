<?php
Fakoli::using("report", "report_manager");


if(!checkRole("admin,data"))
{
	Fakoli::end("Insufficient privileges to delete a report.");
}

$report_id = checkNumeric($_GET["report_id"]);

if ($report_id)
{
	$report = new CustomReport($report_id);
	$report->delete();
}

Fakoli::end("OK");
?>