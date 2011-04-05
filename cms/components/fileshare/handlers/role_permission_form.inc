<?php
Fakoli::using("document", "fileshare", "role");
Fakoli::usingFile("framework/auto_form.inc");

$document_library_id = checkNumeric($_GET["document_library_id"]);

if(!$document_library_id)
	ajaxReturn("invalid parameters");

$library = new DocumentLibrary($document_library_id);
$library->filter = new InclusionFilter("upload_files", "allow_access", "manage_folders");

$sroles = SiteRole::getRolesArray();

foreach($sroles as $role => $name)
{
	if(!preg_match("/$role/", "admin"))
		$roles[$role] = $name;
}


$form = new AutoForm($library, "POST", "/action/fileshare/role_permission_form?document_library_id={$document_library_id}", "RolePermission_form");
$form->ajaxSubmit("function(result) {new LibraryManager().rolesEdited(result);}", "function() {\$('{$form->id}_error').set('text','Failed to communicate with server'); }");
$readCheck = new CheckListFieldRenderer($form, "allow_access", "Read Access", $roles);
$writeCheck = new CheckListFieldRenderer($form, "upload_files", "Upload Access", $roles);
$manageCheck = new CheckListFieldRenderer($form, "manage_folders", "Manage Folders", $roles);
$form->submitLabel = "Update Role Permissions";

if ($method == "POST") 
{
	if($form->save())
	{
		echo "OK";
		return;
	}
	else
	{
		echo $form->msg;
		return;
	}
}

echo $form->writeScript();

$form->drawForm();

class RolePermission extends DataItem
{
	var $fields = array();
	
	function RolePermission()
	{
		$this->primary_key = "role_id";
		$this->table = "role_permission";

		$this->DataItem(func_get_args());
	}
}
?>