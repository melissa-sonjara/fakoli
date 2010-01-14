<?php
require_once "../../include/config.inc";

require_once "../datamodel/site_user.inc";

require_once "../../include/permissions.inc";
require_once "../../framework/auto_form.inc";

$menu_item = "Users";
$user_id = checkNumeric($_GET["user_id"]);

$siteUser = new SiteUser();

$form = new AutoForm($siteUser);
$roleSelect = new SelectFieldRenderer($form, "role", "Role", $siteUserRoles);

if ($method == "POST")
{
	if ($form->save())
	{
		redirect("users.php");	
	}
}

if ($user_id)
{
	$siteUser->load($user_id);
	$title = "Edit Account for {$user->first_name} {$user->last_name}";
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
