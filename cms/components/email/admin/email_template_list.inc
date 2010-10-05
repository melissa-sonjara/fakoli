<?php

/*
 * Title: email_template_list.php
 * 
 * Description: table list view of email template text.
 * 
 * author: Janice Gallant for Sonjara, Inc.
 * 
 * date: 6/7/10
 */


Fakoli::using("email");
Fakoli::usingFeature("data_view");

$emailTemplates = query(EmailTemplate);

$table = new DataListView($emailTemplates, "templates");
$table->column("Name", "<a href=\"/admin/email_template_form?email_template_id={email_template_id}\">{name}</a>", true)
		->column("Recipients", "{recipients}", true)
		->column("Subject", "{subject}", true)
	  	->column("Message", "{message}", true);
	  	
$table->emptyMessage = "There are no email templates defined.";
$table->sortable = true;
$table->pageSize = 4;
$table->filter = true;
$table->cssStyle = "width: 100%";

$script .= $table->writeScript();

$table->drawView();

echo "<p><a class='button' href='/admin/email_template_form'>Add a Template</a></p>\n";
?>