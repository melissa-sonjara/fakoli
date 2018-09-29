<?php
Fakoli::using("report_manager");

$targets = CustomReportManager::getTargets();
if (count($targets) == 0)
{
	Fakoli::end("No Custom Reports have been registered");
}

?>
<p>Please specify the type of data you wish to generate a report for.</p>
<p>
<?
foreach($targets as $target)
{
	$obj = new $target;
	echo "<input type='radio' id='target_type' value='{$target}'> ".$obj->prettifyClassName(true)."<br>";
}
?>
</p>
<a class='button' href='#' onclick='ReportManager.createReportSelect(); return false;'>Create Report</a>