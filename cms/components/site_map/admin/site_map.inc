<?php
/**************************************************************
 * 
 * Description: Displays the site hierarchy
 *
 * author Janice Gallant for Sonjara, Inc.
 * 
 * date: 10/8/2010
 * 
 ****************************************************************/

Fakoli::using("site_map");
Fakoli::usingFeature("tree");

$siteMapPages = query(SiteMap, "ORDER BY identifier");

$siteTree = new TreeControl("site_tree");

$siteTree->height = 300;
$siteTree->target ="Two";

if (count($siteMapPages) > 0)
{
	foreach ($siteMapPages as $l_page) 
	{
		$parent = $l_page->Parent();		
		$node = new TreeNode("siteMapPage_{$l_page->site_map_id}", 
	    	$l_page->identifier, null, false, "tree_node_closed", 
	    	"tree_node_open", "site_map_form?site_map_id={$l_page->site_map_id}&parent_site_map_id={$parent->site_map_id}");
	    $node->leafStyle = "file_node_leaf";
	    $tmp = array( 
	                       'parent' => $parent->site_map_id, 
	                       'node' => $node);
	    $displays[$l_page->site_map_id] = $tmp;
	    
	}	

	
/*
 * If the item does not have a parent, then insert it into the
 * tree as a root node. If it does have a parent, then insert
 * it as a child of that node.
 */	
	if(count($displays) > 0)
	{
		foreach ($displays as $display) 
		{						
			if (!$display['parent'])
			{
				$siteTree->add($display['node']);
			}
			else 
			{
				$parentNode = $displays[ $display['parent'] ]['node'];
				$parentNode->add($display['node']);
			}
		}		
	}
}


?>
<h3>The Site Hierarchy maps the location of pages within the site.</h3>
<table border="0">
 <tr>
  <td colspan="2">
<?
$siteTree->writeHTML();
?>
  </td>
 </tr>
</table>
<br/>
<a class="button" href="/action/site_map/scan"> Scan for Page Updates </a>