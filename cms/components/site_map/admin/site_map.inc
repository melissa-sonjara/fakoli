<?php
/**************************************************************

 Copyright (c) 2010 Sonjara, Inc

 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:

 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.

 Except as contained in this notice, the name(s) of the above 
 copyright holders shall not be used in advertising or otherwise 
 to promote the sale, use or other dealings in this Software 
 without prior written authorization.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.

*****************************************************************/

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

$menu_item = "Site Map";

$title = "Site Map";

$siteMapPages = Query::create(SiteMap, "ORDER BY url")->execute();

$siteTree = new TreeControl("site_tree");

$siteTree->height = 300;
$siteTree->target ="Two";

if (count($siteMapPages) > 0)
{
	foreach ($siteMapPages as $l_page) 
	{
		$parent = $l_page->Parent();		
		$node = new TreeNode("siteMapPage_{$l_page->site_map_id}", 
	    	$l_page->url, null, false, "tree_node_closed", 
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