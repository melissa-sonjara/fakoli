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

$editor = $_GET["Editor"];
$document_library_id = checkNumeric($_GET["document_library_id"]);

$libraries = Query::create(DocumentLibrary, "ORDER BY name")->execute();

if (!$document_library_id && count($libraries) > 0)
{
	$document_library_id = $libraries[0]->document_library_id;
}

$library = new DocumentLibrary();
if ($document_library_id) $library->load($document_library_id);

$docMgr = new DocumentManager($library);

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
<div style="clear: left">
<?
$docMgr->drawSelectView("new LinkPicker().documentSelected()", "new LinkPicker().linkToDocument()", "100%", 240);
?>
<br/>
<button id="linkButton" onclick="new LinkPicker().linkToDocument()" disabled="disabled" class="button">Link to Document</button>
<br/>
</div>