<?php
Fakoli::using("redirect");

$menu_item = "Redirects";

$redirect_id = checkNumeric($_GET["redirect_id"]);

$redirect = new Redirect();

if ($redirect_id)
{
	$redirect->load($redirect_id);
}

$form = new AutoForm($redirect);

$form->hide("user_id");
$form->required("redirect_from", "redirect_to");
$form->allowDelete = true;
$form->annotate("exemption_flag", "Specifies the name of a flag that can be set on a user's session to exempt them from the redirect");
$roleSelect = new CheckListFieldRenderer($form, "exempt_roles", "Exempt Roles", SiteRole::getRolesArray());
$roleSelect->setSize(350, 150, 10);

$form->group("Redirect Settings", "redirect_from", "redirect_to", "user_id", "active", "override", "preserve_query_string");
$form->group("Exemptions", "exemption_flag", "exempt_roles");
$form->makeGroupCollapsible("Exemptions", "override");

if ($method == "POST")
{
	$_POST["user_id"] = $user->user_id;
	if ($form->save())
	{
		Fakoli::notifyOnNextPage($form->deleted ? "Redirect remoted" : "Redirect added");
		Cache::invalidate("resource_overrides");
		redirect("/admin/redirects");
	}
}

$script .= $form->writeScript();
$form->drawForm();
