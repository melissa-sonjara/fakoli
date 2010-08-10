<?php

Fakoli::using("document");
Fakoli::usingFile("framework/data_view.inc");

$libraries = Query::create(DocumentLibrary, "ORDER BY name")->execute();

$table = new DataListView($libraries, "libraries");
$table->column("Library", "<a href='/admin/document_library_form?document_library_id={document_library_id}'>{name}</a>{description}", true, "width: 50%")
	  ->column("Owner", "{getOwnerName()}");
	  
$table->sortable = true;
$table->filter = true;
$table->pageSize = 10;
$table->emptyMessage = "No Document Libraries have been created.";
$script .= $table->writeScript();

$table->drawView();
?>
<br/>
<a href="/admin/document_library_form" class="button">Create a Document Library</a>