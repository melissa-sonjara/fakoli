<?php

require_once "../../include/config.inc";

require_once "../datamodel/page.inc";
require_once "../datamodel/site.inc";
require_once "../../include/permissions.inc";
require_once "../../framework/tree.inc";
require_once "../../framework/grouped_data_view.inc";

$title = "Manage Pages";
$menu_item = "Pages";

$pagesBySite = groupedQuery(Page, "ORDER BY page_title", "site_id");

$sites = query(Site, "ORDER BY site_name");

$pageTable = new GroupedDataListView($pagesBySite, "pages");
$pageTable->column("Title", "<a href='page_form.php?page_id={page_id}'>{page_title}</a>", true, "width: 40%")
		  ->column("Page Identifier", "{identifier}", true, "width: 20%")
		  ->column("Code File", "{php_code_file}", true, "width: 40%");
		  
$pageTable->mode = "tree";
$pageTable->groupBy($sites);
$pageTable->cssStyle = "width: 100%";

$script = $pageTable->writeScript();

/*$pageTree = new TreeControl("page_tree");
$pageTree->width = 500;
$pageTree->height = 500;

foreach($sites as $site)
{
	$siteNode = new Treenode("site[{$site->site_id}]", $site->site_name, null, false, "bare_node_closed", "bare_node_open");

	if (array_key_exists($site->site_id, $pagesBySite))
	{
		$pages = $pagesBySite[$site->site_id];
		
		foreach($pages as $page)
		{
			$childNode =  new TreeNode("page[$page->page_id]", $page->page_title, null, false, "bare_node_closed", 
									   "bare_node_open", "page_form.php?page_id={$page->page_id}");
			$childNode->leafStyle="unchecked_node_leaf";
			$siteNode->add($childNode);			
		}
	}
	
	$pageTree->add($siteNode);
}
$script = $pageTree->writeScript();
*/

require_once admin_begin_page;

$pageTable->drawView();
?>
<br/><br/>
<input type="button" class="button" value=" Add a New Page" onclick="go('page_form.php');">
<?
require_once admin_end_page;
?>