<?php
Fakoli::using("activity_tracker");
Fakoli::usingFeature("data_view");

$menu_item = "Activity Report";

$session_id = $_GET["session_id"];
$showImages = checkNumeric($_GET["showImages"]);
$sessionIdentifier = checkIdentifier($_GET["session"]);

$title = "Session Details";
if (!$showImages)
{
	$constraints .= "AND uri NOT LIKE '/action/image/%'";
}

if ($session_id)
{
	$session = new UserActivitySession($session_id);
}
else if ($sessionIdentifier)
{
	$session = UserActivitySession::fromIdentifier($sessionIdentifier);
}

$activity = ActivityTracker::loadSession($session);

$table = new DataListView($activity, "activity_table");
$table->column("Date", "{activity_time:m/d/Y g:i:s}", true, "width: 20%")
	  ->column("Method", "{method}", true, "width: 10%; text-align: center")
	  ->column("URI", "{uri}", true, "white-space: nowrap; overflow: hidden; text-overflow: ellipsis; width: 40%")
	  ->column("Response Time (s)", "{response_time}", true, "text-align: right")
	  ->column("Memory Usage", "{ActivityTracker::formatMemoryUsage}", true, "text-align:right", Number, null, "{memory_usage}");
	  
$table->filter = true;
$table->pageSize = 50;
$table->sortable = true;
$table->onStartRow = array(new SessionDetailsHelper(), filterImages);
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