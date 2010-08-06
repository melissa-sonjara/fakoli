<?php

Fakoli::using("document");
Fakoli::usingFile("framework/data_view.inc");

Fakoli::assertRole("admin");

$menu_item = "Documents";
$title = "Manage Documents";

$documents = query(Document, "ORDER BY title DESC");

$table = new DataListView($documents, "documents");
$table->column("Title", "<b><a href='document_form?document_id={document_id}'>{title}</b></a>", true, "width: 40%")
	  ->column("Pub Date", "{publication_date}", true, "width: 10%")
	  ->column("Author", "{author}", true, "width: 35%")
	  ->column("Published?", "{public}", true);
	 
$table->filter = true;
$table->sortable = true;
$table->pageSize = 20;
$table->excelFile = "documents";
$table->emptyMessage = "No Documents have been added yet.";

$script .= $table->writeScript();

require_once admin_begin_page;

$table->drawView();
?>
<br/>
<br/>
<a href="document_form" class="button"> Add a New Document </a>
<br/>
<?
require_once admin_end_page;
?>