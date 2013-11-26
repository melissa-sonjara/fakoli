<?php
Fakoli::using("activity_tracker");
Fakoli::usingFeature("data_view");

$menu_item = "Activity Report";

$session_id = $_GET["session_id"];
$showImages = checkNumeric($_GET["showImages"]);

$title = "Session Details";
if (!$showImages)
{
	$constraints .= "AND uri NOT LIKE '/action/image/%'";
}

$session = new UserActivitySession($session_id);

$activity = ActivityTracker::loadSession($session);

$table = new DataListView($activity, "activity_table");
$table->column("Date", "{activity_time}", true, "width: 20%")
	  ->column("Method", "{method}", true, "width: 10%; text-align: center")
	  ->column("URI", "{uri}", true)
	  ->column("Response Time (s)", "{response_time}", true, "text-align: right");
	  
$table->filter = true;
$table->pageSize = 50;
$table->sortable = true;
$table->onStartRow = array(new SessionDetailsHelper(), onStartRow);
$table->excelFile = "session_{$session_id}.xls";

$script .= $table->writeScript();
?>
<form method="GET" action="">
<input type='hidden' name='session_id' value="<?echo $session_id?>"/>
<input type="checkbox" class="checkbox" name="showImages" id="showImages" value="1"<?echo $showImages ? " checked='checked'" : ""?>/> Show Image Records&nbsp;&nbsp;
<input type="submit" class="button" value=" GO "/>
</form>
<?
$table->drawView();
?>