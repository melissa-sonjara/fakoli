<?php
Fakoli::using("error_log", "user");
Fakoli::usingFeature("data_view");

$menu_item = "Error Log";

$title = "Error Log";

$errors = Query::create(ErrorLog, "ORDER BY timestamp DESC")->execute();

$table = new DataListView($errors, "error_log");
$table->column("Date &amp Time", "<a href='error_details?error_log_id={error_log_id}'>{timestamp}</a>", true)
	  ->column("User", "{getUserFullName()}", true)
	  ->column("URI", "{uri}", true)
	  ->column("Message", "{message}", true);
	  
$table->filter = true;
$table->sortable = true;
$table->pageSize = 20;
$table->excelFile = "error_log";
$table->emptyMessage = "No Errors have been logged yet.";

$script .= $table->writeScript();

$table->drawView();
?>