<?php
Fakoli::using("login");

if ($user)
{
	session_destroy();
	unset($user);
}

$mgr = new LoginManager();
$result = $mgr->validateAjaxLogin();

Fakoli::end($result);