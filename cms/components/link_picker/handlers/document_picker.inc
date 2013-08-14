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
Fakoli::usingFeature("data_view");

$editor = $_GET["Editor"];
$document_library_id = checkNumeric($_GET["document_library_id"]);

$libraries = Query::create(DocumentLibrary, "WHERE hidden=0 ORDER BY name")->execute();

if (!$document_library_id && count($libraries) > 0)
{
	$document_library_id = $libraries[0]->document_library_id;
}

$library = new DocumentLibrary();
if ($document_library_id) $library->load($document_library_id);

$docMgr = new DocumentManager($library);

$table = new DataListView($docMgr->documents, "library_documents");

$table->column("Title", array($docMgr, formatLinkPickerTitle), false, "width: 60%")
->column("Author", "{author}", false, "width: 20%; text-align: center")
->column("Uploaded By", "{Owner.first_name} {Owner.last_name}", false, "width: 20%; text-align: center")
->column("Last Modified", "{last_modified:short}", false, "width: 20%; text-align: center")
;

$table->sortable = false;
$table->filter = false;
$table->paginate = false;
$table->emptyMessage = "There are no files in {$library->name}.";
$table->cssStyle = "width: 100%; padding-top: 8px;";

echo $docMgr->writeScript();
echo $table->writeScript();
?>
<div style="border: solid 1px #666; background: url(/fakoli/images/table-header.png); width: 100%;padding: 2px 0px">
 <strong>&nbsp;Document Library: </strong>
 <select name="document_library_id" onchange="new LinkPicker().showLibrary(this.value)">
<?
	foreach($libraries as $library)
	{
		option($library->document_library_id, $library->name, $document_library_id);
	} 
?>
 </select>
</div>
<div style="clear: left; width: 100%;">
<?
echo "<div id='link_picker_document_list' class='scrollbox'>\n";
$table->drawView();

echo "</div>\n";
?>
</div>
<p><button id="linkButton" onclick="new LinkPicker().documentPickerLinkToDocument()" disabled="disabled" class="button">Link to Document</button>
</p>