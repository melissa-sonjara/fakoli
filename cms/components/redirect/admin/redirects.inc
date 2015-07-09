<?php
Fakoli::using("redirect");

$menu_item = "Redirects";

$redirects = Query::create(Redirect, "ORDER BY redirect_from")->execute();

$table = new DataListView($redirects, "redirects");
$table->column("Redirect From", "<a href='redirect_form?redirect_id={redirect_id}'>{redirect_from}</a>", true, "width: 30%")
	  ->column("Redirect To", "<a href='{redirect_to}' target='_blank'>{redirect_to}</a>", true, "width: 30%")
	  ->column("Last Modified", "{last_modified}", true, "width: 20%; text-align: center")
	  ->column("Override", "{override:Yes/No}", true, "width: 10%; text-align: center")
	  ->column("Active", "{active:Yes/No}", true, "width: 10%; text-align: center");

$table->pageSize = 20;
$table->filter = true;
$table->excelFile = "redirects.xls";
$table->emptyMessage = "You have not yet defined any redirects";

$script .= $table->writeScript();
$table->drawView();

?>
<br/>
<a class='button' href='redirect_form'>Add Redirect</a>