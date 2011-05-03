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
Fakoli::using("article", "blog");

$editor = $_GET["Editor"];

$articlesByType = GroupedQuery::create(Article, "ORDER BY title", "article_type")
						->execute();
	
$tree = new TreeControl("articles");
$tree->width = "100%";
$tree->height = "280";
$tree->selectMode = "single";

foreach($articlesByType as $type => $items)
{
	$typeNode = new TreeNode($type, $type, null, false, "tree_node_closed", "tree_node_open");
	
	foreach($items as $item)
	{
		$itemNode = new TreeNode("{$type}_{$item->article_id}", $item->title, null, false, "tree_node_closed", "tree_node_open", "javascript:new LinkPicker().insertLink(\"/article?article_id={$item->article_id}\")");
		$itemNode->leafStyle = "file_node_leaf";
		$typeNode->add($itemNode);
	}
	
	$tree->add($typeNode);
}

$script .= $tree->writeScript();

echo $script;
$tree->writeHTML();
?>