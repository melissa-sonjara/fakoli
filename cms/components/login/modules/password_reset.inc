<?php
Fakoli::using("user");
Fakoli::usingFeature("auto_form");

$p_user = clone($user);

$p_user->filter = new InclusionFilter("password");

$form = new AutoForm($p_user);
$form->getRenderer("password")->forceReset = true;
$script .= $form->writeScript();
$form->submitLabel = "Save New Password";
$form->passwordEncryptor = hashPassword;

if ($method == "POST")
{
	if ($form->save())
	{
		$mgr = UserManager::getSiteUserManager();
		$mgr->loginRedirect($user);
	}
}

?>
<p>Please reset your password using the form below.</p> 
<?
$form->drawForm();
?>