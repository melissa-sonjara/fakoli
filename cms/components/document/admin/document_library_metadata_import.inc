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

Fakoli::using("document", "role");
Fakoli::usingFile("framework/auto_form.inc");

Fakoli::usingFile("framework/tab_bar.inc");

require_once "include/document_library_tabs.inc";

$menu_item = "Documents";

$document_library_id = checkNumeric($_GET["document_library_id"]);

if ($document_library_id)
{
	$library = new DocumentLibrary($document_library_id);
}
else
{
	$library = DocumentManager::createLibraryObj();
}

$tabs = documentLibraryTabs($document_library_id);
$tabs->page = "document_library_form";

$library->filter = new InclusionFilter("document_library_id");
$form = new AutoForm($library);

$form->submitLabel = "Import Metadata";

$upload = new FileUploadFieldRenderer($form, "metadata", "Metadata File", 'uploadMetaData');

if ($method == "POST")
{
	if ($form->save())
	{
		redirect("document_library_form?document_library_id={$document_library_id}");
	}
}


$script .= $form->writeScript();

$tabs->writeHTML();
?>
<div style="clear:left; border: solid 1px #000; width: 100%; margin: 0; padding: 4px;">
<h3><?echo $library->name?></h3>
<?
$form->drawForm();
?>
</div>
<?
function uploadMetaData($field, $library)
{
	$file = $_FILES[$field]["tmp_name"];
	$library = new DocumentLibrary($library->document_library_id);
	$mgr = new DocumentManager($library);
	$mgr->importMetadata($file);
}

?>