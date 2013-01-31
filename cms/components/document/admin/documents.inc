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

Fakoli::using("document");
Fakoli::usingFile("framework/data_view.inc");
require_once "include/document_library_tabs.inc";

Fakoli::assertRole("admin");

$menu_item = "Documents";
$title = "Manage Documents";

$document_library_id = checkNumeric($_GET["document_library_id"]);

if (!$document_library_id)
{
	redirect("/admin/document_libraries");
}

$library = new DocumentLibrary($document_library_id);
$documents = $library->Documents("ORDER BY title DESC");

$table = new DataListView($documents, "documents");
$table->column("Title", "<b><a href='#' onclick='editFile({document_id});return false'>{title}</b></a>", true, "width: 40%")
	  ->column("Author", "{author}", true, "width: 35%");
	 
$table->filter = true;
$table->sortable = true;
$table->pageSize = 20;
$table->excelFile = "documents";
$table->emptyMessage = "No Documents have been added yet.";

ob_start();
?>
<script type="text/javascript">
function editFile(id)
{
	modalPopup("Edit Document Properties",  "/action/document/edit?document_id=" + id, 500);
}

function editResult(result)
{
	if (result == "1")
	{
		window.location.reload();
	}
	else
	{
		$('Document_form__error').set({'text': result, 'display': 'table-cell'});
	}
}
</script>
<?
$script .= ob_get_contents();
ob_end_clean();

$script .= $table->writeScript();
$tabs = documentLibraryTabs($document_library_id);

$tabs->writeHTML();
?>
<div style="clear:left; border: solid 1px #000; width: 100%; margin: 0; padding: 4px;">
<h3>Documents in <?echo $library->name?></h3>
<?
$table->drawView();
?>
<br/>
<br/>
<a href="document_form" class="button"> Add a New Document </a>
<br/>
</div>
