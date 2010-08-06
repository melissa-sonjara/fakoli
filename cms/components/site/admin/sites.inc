<?php

Fakoli::using("site");
Fakoli::usingFile("framework/data_view.inc");

Fakoli::assertRole("admin");

$menu_item = "Sites";
$title = "Manage Sites";


$sites = query(Site, "ORDER BY site_id");

$table = new DataListView($sites, "sites");
$table->column("Title", "<a href='/admin/site_form?site_id={site_id}'>{site_name}</a>", true)
	  ->column("Domain", "<a target='_blank' href='{domain}'>{domain}</a>", true);
$table->sortable = true;

$script .= $table->writeScript();
	  
require_once admin_begin_page;

$table->drawView();
?>
<br/>
<a href="/admin/site_form" class="button">Add a New Site</a>
<br/>
<br/>
<?
require_once admin_end_page;
?>