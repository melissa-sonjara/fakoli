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

Fakoli::usingFeature("tree");
Fakoli::using("section");

$editor = $_GET["Editor"];

$itemsByType = array();
$itemsByType = ComponentManager::fireEvent("EnumerateItems", $itemsByType);

$tree = new TreeControl("items");
$tree->width = "100%";
$tree->height = "300";
$tree->selectMode = "navigation";

$sections = Query::create(Section, "ORDER BY section")->execute();

foreach($sections as $section)
{
	$sectionNode = new TreeNode("section_{$section->section}", $section->section, null, false, "tree_node_closed", "tree_node_open");
	
	$content = $section->Content("ORDER BY identifier");
	$contentByIdentifier = reindexList($content, "identifier");
	
	foreach($itemsByType as $type => $items)
	{
		$category = new TreeNode("content_type_".codify($section->section)."_".codify($type), $type, null, false, "tree_node_closed", "tree_node_open");
	
		$categories[$type] = new TreeNode("category_".codify($section->section)."_".codify($type), $type, null, false, "tree_node_closed", "tree_node_open");
		
		foreach($items as $item)
		{
			if (!array_key_exists($item->identifier, $contentByIdentifier)) continue;
			
			$child = new TreeNode("available[{$item->identifier}]", $item->identifier, null, false, "plain_tree_closed", "plain_tree_open", "javascript:new LinkPicker().insertLink(\"".getLink($section, $item->identifier)."\")");
			$child->leafStyle = "file_node_leaf";
			
			$category->add($child);
		}
		
		$sectionNode->add($category);
	}
	
	$tree->add($sectionNode);
}

$script .= $tree->writeScript();

$tree->writeHTML();

function getLink($section, $identifier)
{
	$url = ($section->section == "/") ? "/$identifier" : "/{$section->section}/$identifier";
	
	return $url;
}
?>