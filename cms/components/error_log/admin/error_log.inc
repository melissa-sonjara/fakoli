<?php
Fakoli::using("error_log", "user");
Fakoli::usingFeature("data_view", "filter_form");

$menu_item = "Error Log";

$title = "Error Log";

$err = new ErrorLog();
$err->filter = new InclusionFilter("timestamp");

$filterForm = new FilterForm($err);
$range = new DateRangeFilterRenderer($filterForm, "timestamp", "timestamp");

$errors = Query::create(ErrorLog, $filterForm->getConstraint()." ORDER BY timestamp DESC")->execute();

$filterForm->drawForm();

$table = new DataListView($errors, "error_log");
$table->column("Date", "<a href='error_details?error_log_id={error_log_id}'>{timestamp}</a>", true)
	  ->column("User", "{getUserFullName()}", true)
	  ->column("URI", "{uri:htmlSafe}", true)
	  ->column("Message", "{message:htmlSafe}", true);
	  
$table->filter = true;
$table->sortable = true;
$table->pageSize = 20;
$table->excelFile = "error_log";
$table->emptyMessage = "No Errors have been logged yet.";

$script .= $table->writeScript();

$table->drawView();
?>