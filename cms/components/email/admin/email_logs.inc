<?php
Fakoli::using("email");
Fakoli::usingFeature("filter_form");

$menu_item = "Email Logs";

$emailLog = new EmailLog();
$emailLog->filter = new InclusionFilter("date_sent");
$emailLog->fromGET();

$filterForm = new FilterForm($emailLog, "GET");
$filterForm->hide("startDate");

$dateRangeRenderer = new DateRangeFilterRenderer($filterForm, "date_sent", "date_sent", "year");
$title = "Email Logs for ". $dateRangeRenderer->subTitle;

$logs = Query::create(EmailLog, "ORDER BY date_sent DESC")->execute();

$table = new DataListView($logs, "EmailLogs");
$table->column("Recipient", "{recipient_name} &lt;{recipient_email}&gt;", true)
	  ->column("Sender", "{sender_name} &lt;{sender_email}&gt;", true)
	  ->column("Subject", "{subject}", true)
	  ->column("Date Sent", "{date_sent:short}", true);

$table->filter = true;
$table->pageSize = 10;
$table->sortable = true;
$table->excelFile = "email_logs.xls";
$table->emptyMessage = "There are no email logs for " .$dateRangeRenderer->subTitle;

$script .= $filterForm->writeScript();
$script .= $table->writeScript();

$filterForm->drawForm();
$table->drawView();

?>