<?php
Fakoli::using("activity_tracker");
Fakoli::usingFeature("data_view", "filter_form");

$feedbackUsers = checkNumeric($_GET["feedbackUsers"]);

$menu_item = "Activity Report";

$helper = new ActivityReportFilterHelper();
$filterForm = $helper->setup();

$constraint = $filterForm->getConstraint();

$sessions = IteratedQuery::create(UserActivitySession, "$constraint ORDER BY session_start DESC")->execute();

$table = new DataListView($sessions, "session");
$table->column("User", "<a href='/admin/session_details?session_id={session_id}'>{User.first_name} {User.last_name}</a>", true, "width: 25%")
	  ->column("Session #", "<a href='/admin/session_details?session_id={session_id}'>{session_id}</a>", true, "text-align: right; width: 5%")
	  ->column("Start", "{session_start}", true)
	  ->column("End", "{session_end}", true)
	  ->column("Page Views", "{page_views}", true, "text-align: right")
	  ->column("Feedback", "{feedback_count}", true, "text-align: right")
	  ->column("Browser", "{ActivityTracker::getBrowser}", true, "width: 20%");
	  
$table->sortable = true;
$table->pageSize = 20;
$table->filter = true;
$table->excelFile = "activity_report.xls";

$script .= $table->writeScript();
$script .= $filterForm->writeScript();

$filterForm->drawForm();
echo "<strong>" . $filterForm->getRenderer("session_start")->subTitle . "</strong>";
$table->drawView();

$count = Query::create(UserActivity, "")->executeValue("COUNT(1)");

if ($count > 0)
{
?>
<br/>
<a class='button' href='#' onclick='new BackgroundProcess("Migrating User Activity", "/action/activity_tracker/migrate_data"); return false;'>Migrate User Activity Logs</a>
<?
}
?>
