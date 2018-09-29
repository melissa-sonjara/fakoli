<?php
Fakoli::using("report");
Fakoli::includeScript("/components/report_manager/js/report_background_loader.js");

$report_id = $_GET["report_id"];
$excel = $_GET["__excel"];

if ($report_id && is_numeric($report_id) && $method == "GET")
{
	$report = new CustomReport($report_id);
	$page->page_title = $report->title;
	$method = "reportLoader.loadFromID($report_id, '$excel');";
}
else
{
	$method = "reportLoader.loadFromRequest(".json_encode($_POST).", '$excel');";
}

$script .= <<<ENDSCRIPT

<script type="text/javascript">
window.addEvent('load', function()
{
	var reportLoader = new ReportBackgroundLoader('report_body');
	$method
});
</script>
ENDSCRIPT;
?>
<div id="report_body" style="width:100%">

 <div id="report_interstitial" style="margin: 150px">
 <img src="/fakoli/images/loader.gif" style='display:inline-block;vertical-align: middle'/><span style="font-size: 24pt">&nbsp;&nbsp;Generating Report...</span>
 </div>
</div>