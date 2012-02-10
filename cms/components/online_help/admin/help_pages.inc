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

$help_book_id = checkNumeric($_GET["help_book_id"]);
if (!$help_book_id) redirect("help_books");

$menu_item = "Online Help";

$helpBook = new HelpBook($help_book_id);
$title = "Help Pages for {$helpBook->title}";

$tabs = OnlineHelpManager::getAdminTabs($help_book_id);

$helppages = Query::create(HelpPage, "ORDER BY sort_order")->execute();

$helpTree = new TreeControl("help_tree");

$helpTree->height = 300;

if (count($helppages) > 0)
{
	foreach ($helppages as $helppage) 
	{
	    $help_pageNode = new TreeNode("helppage_{$helppage->help_page_id}", $helppage->title, null, false, "tree_node_closed", "tree_node_open", "help_page_form?help_book_id={$help_book_id}&help_page_id={$helppage->help_page_id}&parent_id={$helppage->parent_id}");
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
$tabs->writeHTML();
?>
<div style="clear:left; border: solid 1px #000; width: 100%; margin: 0; padding: 4px;">
<h3><?echo $helpBook->title?></h3>
<?
$helpTree->writeHTML();

?>
<br/>
<a class="button" href="help_page_form?help_book_id=<?echo $help_book_id?>&parent_id=0">Add a New Help Page</a>
</div>
