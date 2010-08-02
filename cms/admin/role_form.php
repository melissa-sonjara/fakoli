<?php
require_once "../../include/config.inc";
require_once "../../include/permissions.inc";
require_once "../../framework/auto_form.inc";

$menu_item = "Roles";
$role_id = checkNumeric($_GET["role_id"]);

$role = new SiteRole();

$form = new AutoForm($role);
$redirect = "roles.php";
$form->unique("role", "That role has already been defined");
$form->required("role", "name");

$form->button("Cancel", $redirect);

if ($method == "POST")
{
	if ($form->save())
	{
		redirect($redirect);	
	}
}

if ($role_id)
{
	$role->load($role_id);
	$title = "Edit User Role: {$role->name}";
}
else
{
	$title = "Add a New User Role";
}

$script .= $form->writeScript();

require_once admin_begin_page;

$form->drawForm();

require_once admin_end_page;
?>
