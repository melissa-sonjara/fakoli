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

$form = DocumentManager::buildDocumentLibraryForm($library);

$form->button("Import Metadata...", "document_library_metadata_import?document_library_id={$document_library_id}");

if ($document_library_id)
{
	$form->button("Download ZIP Archive", 
		"new BackgroundProcess('Creating ZIP Archive', '/action/document/zip_library?document_library_id={$document_library_id}', ".
        "{onComplete: function() { window.location.href = '/action/document/zip_library_download?document_library_id={$document_library_id}'; } });". 
        "return false;", false, true);
}

if ($method == "POST")
{
	if ($form->save())
	{
		DocumentManager::createLibrary($form);
		
		$tabs->queryString = "document_library_id=".$form->data->document_library_id;
		redirect("/admin/document_library_permissions?document_library_id={$form->data->document_library_id}");
	}
}


$script .= $form->writeScript();

$tabs->writeHTML();
?>
<div style="clear:left; border: solid 1px #000; width: 100%; margin: 0; padding: 4px;">
<?
$form->drawForm();

function deleteDocumentLibary($library)
{
	$mgr = new DocumentManager($library);
	$mgr->deleteDocumentLibrary();	
	redirect("/admin/document_libraries");
}
?>
</div>