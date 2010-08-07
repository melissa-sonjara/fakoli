<?php
Fakoli::using("role");
Fakoli::usingFile("framework/auto_form.inc");
Fakoli::assertRole("admin");

$menu_item = "Roles";
$role_id = checkNumeric($_GET["role_id"]);

$role = new SiteRole();

$form = new AutoForm($role);
$redirect = "/admin/roles";
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



$form->drawForm();


?>
