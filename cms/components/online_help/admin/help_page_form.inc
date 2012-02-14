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
Fakoli::usingFeature("auto_form", "tab_bar");

$menu_item = "Online Help";

$help_book_id = checkNumeric($_GET["help_book_id"]);
if (!$help_book_id) redirect("help_books");

$help_page_id = checkNumeric($_GET["help_page_id"]);
$parent_id = checkNumeric($_GET["parent_id"]);

$helpBook = new HelpBook($help_book_id);
$title = "Help Page for {$helpBook->title}";

$page = new HelpPage();

if ($help_page_id)
{
	$page->load($help_page_id);
}
else
{
	$page->parent_id = $parent_id;
	$page->help_book_id = $help_book_id;
}

$form = new AutoForm($page);
$form->required("title", "identifier");
$form->hide("help_book_id", "parent_id");

if ($method == "POST")
{
	if ($_POST["reorder_subitems"])
	{
	   	$sort_nums = checkNumeric($_POST["m"]);
		
		foreach($sort_nums as $key => $sort_num)
		{
			$p = new HelpPage();
			$p->filter = new InclusionFilter("help_page_id", "sort_order");
			$p->load($key);
			$p->sort_order = $sort_num;
			$p->save();
		}		
	}
	else if ($form->save())
	{
		redirect("help_pages?help_book_id={$help_book_id}");
	}
}


if ($help_page_id)
{
	$title = "Edit Help Page Details for {$page->title}";
	
	$subpages = $page->Children();
	if(count($subpages) > 0) 
	{
		$form->allowDelete = false;
	}
	else 
	{ 
		$form->allowDelete = true; 
	}
	
	$form->button("Move Help Page", "/admin/help_page_move_form?help_book_id={$help_book_id}&help_page_id={$help_page_id}&parent_id={$parent_id}");
	$form->button("Cancel", $redirect);
}
else
{
	$title = "Add a new Help Page";
	$form->button("Cancel", $redirect);
}


$tabs = OnlineHelpManager::getAdminTabs($help_book_id);
$tabs->page = "/admin/help_pages";

$script = $form->writeScript();

$tabs->writeHTML();
?>
<div style="clear:left; border: solid 1px #000; width: 100%; margin: 0; padding: 4px;">
<?
$form->drawForm();

if ($help_page_id)
{
?>
<h4>Sub-pages</h4>
<?
	if ($subpages)
	{
?>

<form name='help_page_order_form' method="POST" action="">
<input type="hidden" name="reorder_subitems" value="1"/>
<table class="list" style='width: 90%; margin-left: 4%'>
 <tr>
  <th>Sort Order</th>
  <th>Help Page</th>
 </tr>
<?
		foreach($subpages as $p)
		{
?>
 <tr>
  <td style="text-align: center; vertical-align: middle; width: 80px"><input type="text" style="width: 60px" name="m[<?echo $p->help_page_id ?>]" value="<?echo $p->sort_order ?>"/></td>
  <td><a href="/admin/help_page_form?help_page_id=<?echo $p->help_page_id?>"><?echo $p->title ?></a></td>
 </tr>
<?
		}
?>
</table>

<br/>

<input type="submit" class="button" value="Update Page Order"/>&nbsp;&nbsp;<a href="help_page_form?help_book_id=<?echo $helpBook->help_book_id?>&parent_id=<?echo $help_page_id?>" class="button">Add a new Sub-page</a>

</form>
<?
	}
	else
	{
?><p><i>There are no sub-pages associated with this help page.</i></p>
<a href="/admin/help_page_form?help_book_id=<?echo $helpBook->help_book_id?>&parent_id=<?echo $help_page_id?>" class="button">Add a new Sub-page</a>
<?
	}
}
?>
</div>
