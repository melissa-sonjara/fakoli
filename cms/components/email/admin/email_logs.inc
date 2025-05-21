<?php
Fakoli::using("email");
Fakoli::usingFeature("filter_form");

$menu_item = "Email Logs";

$emailLog = new EmailLog();
$emailLog->filter = new InclusionFilter("date_sent");
$emailLog->fromGET();

$filterForm = new FilterForm($emailLog, "GET");
$filterForm->hide("startDate");

$dateRangeRenderer = new DateRangeFilterRenderer($filterForm, "date_sent", "date_sent");
$title = "Email Logs for ". $dateRangeRenderer->subTitle;

$constraint = $filterForm->getConstraint();

$logs = Query::create('EmailLog', "$constraint ORDER BY date_sent DESC")->execute();

$table = new DataListView($logs, "EmailLogs");
$table->column("Recipient Email", "{recipient_email}", true)
	  ->column("Sender Email", "{sender_email}", true)
	  ->column("Subject", "{subject}", true)
	  ->column("Body", "<a href='#' onclick='new EmailManager().showMessageLog({email_log_id})'><img src='/fakoli/images/documents.gif' alt='Show Message Body'/></a>", false, "text-align: center; width: 10%")
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
