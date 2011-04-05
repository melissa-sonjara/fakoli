<?php
Fakoli::using("document", "fileshare", "role");
Fakoli::usingFile("framework/auto_form.inc");

$document_library_id = checkNumeric($_GET["document_library_id"]);

if(!$document_library_id)
	ajaxReturn("invalid parameters");

$library = new DocumentLibrary($document_library_id);
$library->filter = new InclusionFilter("");

$roles = Query::create(SiteRole, "ORDER BY role")->execute();

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