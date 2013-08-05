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
->column("Published", "{published:Yes/No}", true, "text-align: center");

$table->enableDragReorder("/action/link_library/reorder_links");
$table->dragText = "<span style='font-size: 10px'>Click and drag to reorder links</span>";

$script .= $table->writeScript();

$table->drawView();