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

Fakoli::using("online_help");
Fakoli::usingFeature("tree");


$book = $_GET["book"];
$page = $_GET["page"];

$helpBook = HelpBook::findByIdentifier($book);
$page = $helpBook->findPage($page);

$helppages = Query::create(HelpPage, "WHERE help_book_id=:id AND public=1 ORDER BY sort_order")
				  ->bind(":id", $helpBook->help_book_id)
				  ->execute();

$helpTree = new TreeControl("help_tree");
$helpTree->scroll = false;
$helpTree->width = 0;
$helpTree->style = "help_tree";

if (count($helppages) > 0)
{
	foreach ($helppages as $helppage) 
	{
	    $help_pageNode = new TreeNode("helppage_{$helppage->help_page_id}", $helppage->title, null, false, "tree_node_closed", "tree_node_open", "/action/online_help/page?book={$book}&page={$helppage->identifier}", "Two");
	    $help_pageNode->leafStyle = "file_node_leaf";
	    $tmp = array('parent' => $helppage->parent_id, 
	                 'node' => $help_pageNode);
	    $displays[$helppage->help_page_id] = $tmp;
	}	

	foreach ($displays as $display) 
	{	
		if (empty($display['parent'])) 
		{
			$helpTree->add($display['node']);
		}
		else 
		{
			$parentNode = $displays[ $display['parent'] ]['node'];
			$parentNode->add($display['node']);
		}
	}
}


$script = $helpTree->writeScript();

$mgr = new OnlineHelpManager($helpBook);

$mgr->startFrame();

$helpTree->writeHTML();

$mgr->endFrame();
?>