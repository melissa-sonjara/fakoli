<?php

Fakoli::using("user", "role");
Fakoli::usingFile("framework/auto_form.inc");

$menu_item = "Users";
$user_id = checkNumeric($_GET["user_id"]);

$siteUser = new SiteUser();

$form = new AutoForm($siteUser);
$roleSelect = new ChecklistFieldRenderer($form, "role", "Role", SiteRole::getRolesArray());
$roleSelect->setSize(323, 120);

$redirect = "/admin/users";
$form->unique("email", "That email address is already in use for another user");
$form->hide("composite_class");
$form->button("Cancel", $redirect);

if ($method == "POST")
{
	if ($form->save())
	{
		redirect($redirect);	
	}
}

if ($user_id)
{
	$siteUser->load($user_id);
	$title = "Edit Account for {$siteUser->first_name} {$siteUser->last_name}";
}
else
{
	$title = "Add a New User Account";
}

$script .= $form->writeScript();



$form->drawForm();


?>
