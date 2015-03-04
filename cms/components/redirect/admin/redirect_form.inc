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

if ($method == "POST")
{
	$_POST["user_id"] = $user->user_id;
	if ($form->save())
	{
		Fakoli::notifyOnNextPage("Redirect added");
		redirect("/admin/redirects");
	}
}

$script .= $form->writeScript();
$form->drawForm();
