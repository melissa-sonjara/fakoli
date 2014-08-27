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

/*
 * Let's library owner set global permissions:
 * 
 *  1) Allow only members to view
 *  
 *  2) Allow all site members to view
 *  
 *  3) Allow visitors to view
 * 
 */

Fakoli::using("document", "fileshare", "role");
Fakoli::usingFile("framework/auto_form.inc");

$document_library_id = checkNumeric($_GET["document_library_id"]);

if(!$document_library_id)
	Fakoli::end("invalid parameters");

$library = new DocumentLibrary($document_library_id);
$library->filter = new InclusionFilter("");

$roles = Query::create(SiteRole, "ORDER BY name")->execute();

if(count($roles) > 0)
	$allRoles = implode(",", array_keys(reindexList($roles, "role")));

$form = new AutoForm($library, "POST", "/action/fileshare/global_permissions_form?document_library_id={$document_library_id}", "RolePermission_form");
$form->ajaxSubmit("function(result) {new LibraryManager().permissionsEdited(result);}", "function() {document.id('{$form->id}_error').set('text','Failed to communicate with server'); }");

$permissionsSelect = new RadioButtonFieldRenderer($form, "read_access", "Permissions", FileshareManager::getGlobalPermissionOptions());


if(preg_match("/visitor/", $library->allow_access))
	$form->data->set("read_access", fileshare_all_visitors_read);
elseif(preg_match("/$allRoles/", $library->allow_access))
	$form->data->set("read_access", fileshare_all_site_members_read);
else
	$form->data->set("read_access", fileshare_only_group_members_read);

$form->submitLabel = "Update Access";

if ($method == "POST") 
{
	$read = $_POST["read_access"];

	if($read == fileshare_only_group_members_read)
		$library->allow_access = DocumentLibrary::getDefaultRole(true);
	elseif($read == fileshare_all_site_members_read)
		$library->allow_access = $allRoles;
	else
		$library->allow_access = DocumentManager::getRoleList();
	$library->filter = new InclusionFilter("allow_access");
	$library->save();
	
	echo "OK";
	return;
	// no error conditions;
}

echo $form->writeScript();

$form->drawForm();
?>