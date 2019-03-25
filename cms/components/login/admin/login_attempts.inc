<?php
Fakoli::using("login");

$menu_item = "Login Attempts";

$attempt = new LoginAttempt();

$attempt->filter = new InclusionFilter("date_received");
$attempt->fromGET();
$filterForm = new FilterForm($attempt);
new DateRangeFilterRenderer($filterForm, "date_received", "", "month");

$constraint = $filterForm->getConstraint();

$attempts = Query::create(LoginAttempt, "$constraint ORDER BY date_received")->execute();

$table = new DataListView($attempts, "login_attempts");
$table->column("Username", "<b>{username:htmlSafe}</b>", true)
	  ->column("Date", "{date_received}", true, "text-align: center", null, null, "{date_received:YmdHis}")
	  ->column("IP Address", "{ip_address}", true, "text-align: center")
	  ->column("Login Type", "{login_mode:prettify}", true, "text-align: center")
	  ->column("Result", "{result:prettify}", true, "text-align: center");

$table->pageSize = 20;
$table->sortable = true;
$table->filter = true;
$table->emptyMessage = "There have been no login attempts for this time period";
$table->excelFile = "login_attempts.xls";

$script .= $filterForm->writeScript();
$filterForm->drawForm();

$script .= $table->writeScript();
$table->drawView();