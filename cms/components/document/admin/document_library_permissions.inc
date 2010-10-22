<?
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

require_once "include/document_library_tabs.inc";

$menu_item = "Documents";

$document_library_id = checkNumeric($_GET["document_library_id"]);

if (!$document_library_id)
{
	redirect("/admin/document_libraries");
}

$library = new DocumentLibrary($document_library_id);

$library->filter = new InclusionFilter("document_library_id", "manage_folders", "upload_files", "delete_files", "last_modified");

$tabs = documentLibraryTabs($document_library_id);

$form = new AutoForm($library);

$manageFoldersList = new CheckListFieldRenderer($form, "manage_folders", "Create & Delete Folders", SiteRole::getRolesArray());
$uploadFilesList   = new CheckListFieldRenderer($form, "upload_files", "Upload Files", SiteRole::getRolesArray());
$deleteFilesList   = new CheckListFieldRenderer($form, "delete_files", "Delete Files", SiteRole::getRolesArray());

if ($method == "POST")
{
	if ($form->save())
	{
		$tabs->next();
	}
}

$script .= $form->writeScript();

$tabs->writeHTML();
?>
<div style="clear:left; border: solid 1px #000; width: 100%; margin: 0; padding: 4px;">
<h3>Permissions for <?echo $library->name?></h3>
<?
$form->drawForm();
?>
</div>