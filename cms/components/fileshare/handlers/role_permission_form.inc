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

Fakoli::using("document", "fileshare", "role");
Fakoli::usingFile("framework/auto_form.inc");

$document_library_id = checkNumeric($_GET["document_library_id"]);

if(!$document_library_id)
	ajaxReturn("invalid parameters");

$library = new DocumentLibrary($document_library_id);
$library->filter = new InclusionFilter("");

$roles = Query::create(SiteRole, "ORDER BY name")->execute();

if(count($roles) > 0)
	$allRoles = implode(",", array_keys(reindexList($roles, "role")));

$form = new AutoForm($library, "POST", "/action/fileshare/role_permission_form?document_library_id={$document_library_id}", "RolePermission_form");
$form->ajaxSubmit("function(result) {new LibraryManager().rolesEdited(result);}", "function() {\$('{$form->id}_error').set('text','Failed to communicate with server'); }");
$form->add(new BooleanFieldRenderer($form), "read_access");
$form->alias("read_access", "Allow All Site Members to View");
if(preg_match("/$allRoles/", $library->allow_access))
	$form->data->set("read_access", true);

$form->submitLabel = "Update Access";

if ($method == "POST") 
{
	$read = $_POST["read_access"];

	if($read)
		$library->allow_access = $allRoles;
	else
		$library->allow_access = DocumentLibrary::getDefaultRole(true);
	$library->filter = new InclusionFilter("allow_access");
	$library->save();
	
	echo "OK";
	return;
	// no error conditions;
}

echo $form->writeScript();

$form->drawForm();
?>