<?php
Fakoli::using("activity_tracker");
Fakoli::usingFeature("data_view", "filter_form");

$feedbackUsers = checkNumeric($_GET["feedbackUsers"]);

$menu_item = "Activity Report";

$helper = new ActivityReportFilterHelper();
$filterForm = $helper->setup();

$constraint = $filterForm->getConstraint();

$sessions = IteratedQuery::create(ActivitySession, "$constraint ORDER BY session_start DESC")->execute();

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
$script .= $filterForm->writeScript();

$filterForm->drawForm();
echo "<strong>" . $filterForm->getRenderer("session_start")->subTitle . "</strong>";
$table->drawView();

?>
