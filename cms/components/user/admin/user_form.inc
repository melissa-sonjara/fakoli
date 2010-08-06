<?php

Fakoli::using("user", "role");
Fakoli::usingFile("framework/auto_form.inc");

$menu_item = "Users";
$site_user_id = checkNumeric($_GET["site_user_id"]);

$siteUser = new SiteUser();

$form = new AutoForm($siteUser);
$roleSelect = new SelectFieldRenderer($form, "role", "Role", $siteUserRoles);
$redirect = "users.php";
$form->unique("email", "That email address is already in use for another user");
$form->button("Cancel", $redirect);

if ($method == "POST")
{
	if ($form->save())
	{
		redirect("users.php");	
	}
}

if ($site_user_id)
{
	$siteUser->load($site_user_id);
	$title = "Edit Account for {$siteUser->first_name} {$siteUser->last_name}";
}
else
{
	$title = "Add a New User Account";
}

$script .= $form->writeScript();

require_once admin_begin_page;

$form->drawForm();

require_once admin_end_page;
?>
