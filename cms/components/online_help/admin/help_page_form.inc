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
Fakoli::usingFeature("auto_form", "tab_bar", "data_view");

$menu_item = "Online Help";

$help_book_id = checkNumeric($_GET["help_book_id"]);
if (!$help_book_id) redirect("help_books");

$help_page_id = checkNumeric($_GET["help_page_id"]);
$parent_id = checkNumeric($_GET["parent_id"]);

$helpBook = new HelpBook($help_book_id);
$title = "Help Page for {$helpBook->title}";

$helpPage = new HelpPage();
$subpage = array();

if ($help_page_id)
{
	$helpPage->load($help_page_id);
	$title = "Edit Help Page Details for {$helpPage->title}";
	$subpages = $helpPage->Children();
}
else
{
	if($parent_id)
	{
		$helpPage->parent_id = $parent_id;
	}
	$helpPage->sort_order = Query::create(HelpPage, "WHERE help_book_id=:help_book_id")
		->bind(":help_book_id", $help_book_id)
		->executeValue("MAX(sort_order)") + 1;
	$helpPage->help_book_id = $help_book_id;
	$title = "Add a new Help Page";
}


$form = new AutoForm($helpPage);
$form->required("title", "identifier");
$form->hide("help_book_id", "sort_order");

$form->allowDelete = count($subpages) ? false : true;
	
if($help_page_id && !count($subpages))
{
	$form->button("Add a Subpage", "help_page_form?help_book_id=$help_book_id&parent_id=$help_page_id");
}

$parentSelect = new SelectFieldRenderer($form, "parent_id", "Parent Page", OnlineHelpHelper::formatParentOptions($helpBook, $helpPage));
$parentSelect->defaultBlank = true;
$parentSelect->defaultText = "";

$form->layout = new UngroupedFormLayout($form);
$form->group("all", "title", "identifier", "parent_id");

$form->button("Cancel", $redirect);

$table = OnlineHelpHelper::buildPageTable($subpages, $help_book_id);
$table->emptyMessage = "There are no subpages.";


if ($method == "POST")
{
	if ($form->save())
	{
		redirect("help_pages?help_book_id={$help_book_id}");
	}
}


$tabs = OnlineHelpManager::getAdminTabs($help_book_id);
$tabs->page = "/admin/help_pages";

$script = $form->writeScript();
$script .= $table->writeScript();

$tabs->writeHTML();
?>
<div id="tab_border">
<?
$form->drawForm();

if(count($subpages))
{
	echo "<h3>Subpages</h3>\n";
	$table->drawView();
	echo "<p><a class='button' href='help_page_form?help_book_id=$help_book_id&parent_id=$help_page_id'>Add a Subpage</a></p>\n";
}
?>
</div>