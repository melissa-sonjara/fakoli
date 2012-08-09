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

Fakoli::using("document", "role", "fileshare");
Fakoli::usingFile("framework/auto_form.inc");
Fakoli::usingFile("framework/tab_bar.inc");

$document_library_id = checkNumeric($_GET["document_library_id"]);

if ($document_library_id)
{
	$library = new DocumentLibrary($document_library_id);
	if(!$library->isOwner() && !checkRole("fileshare,admin"))
		redirect("/fileshare_libraries");
}
else
{
	$library = DocumentManager::createLibraryObj();
}

$form = new AutoForm($library);
$form->hide("owner_id", "allow_access", "manage_folders", "upload_files", "delete_files", "create_date", "identifier");
$form->required("name");
$form->onSaveComplete = setLibraryIdentifier;
$form->unique("name", "A library already exists with that name. Please choose a different name.");

$tabs = FileshareManager::libraryTabs($document_library_id);
//$styles .= FileshareManager::getStyles();

if ($method == "POST")
{
	if ($form->save())
	{
		DocumentManager::createLibrary($form);
				
		$tabs->queryString = "document_library_id=".$form->data->document_library_id;
		redirect("fileshare_library_members?document_library_id=".$form->data->document_library_id);
	}
}


$script .= $form->writeScript();
$tabs->writeHTML();
?>
<div id="tab_border">
<?php
$form->drawForm();

function setLibraryIdentifier(&$form)
{
	FileshareManager::setLibraryIdentifier($form);
}
?></div>