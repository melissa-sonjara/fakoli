<?php
Fakoli::using("activity_tracker");
Fakoli::usingFeature("data_view");

$feedbackUsers = checkNumeric($_GET["feedbackUsers"]);

$menu_item = "Activity Report";

$constraints = "WHERE 1=1";

if ($feedbackUsers)
{
	$constraints .= " AND session_id IN (SELECT DISTINCT session_id from user_activity WHERE uri like '/action/feedback/save%')";
}

$sessions = Query::create(ActivitySession, "$constraints ORDER BY session_start DESC")->execute();

$table = new DataListView($sessions, "session");
$table->column("Session", "<a href='/admin/session_details?session_id={session_id}'>{User.first_name} {User.last_name}<br/>{session_id}</a>", true, "width: 40%")
	  ->column("Start", "{session_start}", true)
	  ->column("End", "{session_end}", true)
	  ->column("Page Views", "{page_views}", true, "text-align: right")
	  ->column("Feedback Msgs", "{num_feedback}", true, "text-align: right");
	  
$table->sortable = true;
$table->pageSize = 20;
$table->filter = true;
$table->excelFile = "activity_report.xls";

$script .= $table->writeScript();
?>
<form method="GET" action="">
<input type="checkbox" class="checkbox" name="feedbackUsers" id="feedbackUsers" value="1"<?echo $feedbackUsers ? " checked='checked'" : ""?>/> Show only users that provided feedback&nbsp;&nbsp;
<input type="submit" class="button" value=" GO "/>
</form>
<?
$table->drawView();
?>
