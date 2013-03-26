<?php
Fakoli::using("link_library");
Fakoli::usingFeature("data_view");

Fakoli::assertRole("admin,editor");

$link_library_id = checkNumeric($_GET["link_library_id"]);

$library = new LinkLibrary($link_library_id);
$links = $library->Links("ORDER BY title, url");

$table = new DataListView($links, "link_records");
$table->column("Link Title", "<a href='#' onclick='LinkLibraryManager.editLink({link_id})'>{title}</a>", true)
->column("URL", "<a href='{url}' target='_blank'>{url}</a>", true)
->column("Description", "{description:100}", true)
->column("Published", "{published:Yes/No}", true);

$table->pageSize = 20;
$table->filter = true;
$table->sortable = true;
$table->zebra = true;
$table->emptyMessage = "No links have been added to this library yet.";

$script .= $table->writeScript();

$table->drawView();