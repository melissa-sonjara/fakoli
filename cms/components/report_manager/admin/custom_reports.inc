<?php
Fakoli::using("report", "text_lookup");

$page->page_title = "Custom Reports";

$common = new CustomReportListView("Most Commonly Used Reports", "common_reports", "shared = 1", 2);
$myReports = new CustomReportListView("My Reports", "my_reports", "user_id={$user->user_id}", 4);

$script .= $common->writeScript();
$script .= $myReports->writeScript();

echo TextLookup::getText("custom_reports_intro");

$common->drawView();
$myReports->drawView();

?>
<p><a class='button' href='#' onclick='ReportManager.createReportDialog();return false;'><i class='fas fa-plus'></i> Create a New Report</a></p> 
