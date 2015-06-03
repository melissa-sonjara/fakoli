<?php
Fakoli::using("activity_tracker");
Fakoli::usingFeature("data_view");

$menu_item = "Feedback";

$feedback = Query::create(UserFeedback, "ORDER BY created_date DESC")->execute();

$table = new DataListView($feedback, "feedback_table");
$table->column("Date", "{created_date}", true, "width: 15%")
	  ->column("User & Session", "<a href='/admin/session_details?session={session}'>{User.first_name} {User.last_name}<br/>{session}</a>", true, "width:25%")
	  ->column("Feedback", "{feedback}", false, "width: 35%")
	  ->column("URI", "{referer}", false, "width: 25%")
	  ;

$table->filter = true;
$table->pageSize = 20;
$table->sortable = true;
$table->excelFile = "feedback.xls";

$script .= $table->writeScript();

$table->drawView();
?>