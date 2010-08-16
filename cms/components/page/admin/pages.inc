<?php

Fakoli::using("page");
Fakoli::using("site");

Fakoli::assertRole("admin");

Fakoli::usingFile("framework/tree.inc");
Fakoli::usingFile("framework/grouped_data_view.inc");

$title = "Manage Pages";
$menu_item = "Pages";

$pagesBySite = groupedQuery(Page, "ORDER BY page_title", "site_id");

$sites = query(Site, "ORDER BY site_name");

$pageTable = new GroupedDataListView($pagesBySite, "pages");
$pageTable->column("Title", "<a href='/admin/page_form?page_id={page_id}'>{page_title}</a>", true, "width: 40%")
		  ->column("Page Identifier", "{identifier}", true, "width: 20%")
		  ->column("Code File", "{php_code_file}", true, "width: 40%");
		  
$pageTable->mode = "tree";
$pageTable->groupBy($sites);
$pageTable->cssStyle = "width: 100%";

$script = $pageTable->writeScript();

$pageTable->drawView();
?>
<br/><br/>
<input type="button" class="button" value=" Add a New Page" onclick="go('/admin/page_form');">
<?

?>