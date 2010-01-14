<?php

require_once "../../include/config.inc";

require_once "../datamodel/page.inc";
require_once "../datamodel/site.inc";
require_once "../../include/permissions.inc";
require_once "../../framework/tree.inc";

$title = "Manage Pages";
$menu_item = "Pages";

$pagesBySite = groupedQuery(Page, "ORDER BY page_title", "site_id");

$sites = query(Site, "ORDER BY site_name");

$pageTree = new TreeControl("page_tree");
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

require_once admin_begin_page;
?>
<table border="0">
 <tr>
  <td colspan="2">
<?
$pageTree->writeHTML();
?>
  </td>
 </tr>
 <tr>
  <td colspan="2">&nbsp;</td>
 </tr>
 <tr>
  <td align="left"><input type="button" class="button" value=" Add a New Page" onclick="go('page_form.php');"></td>
  
 </tr>
</table>

<?
require_once admin_end_page;
?>