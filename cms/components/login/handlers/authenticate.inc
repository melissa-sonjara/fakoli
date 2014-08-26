<?php
Fakoli::using("login");

if ($user)
{
	Fakoli::end("ERROR|Already logged in");
}

$mgr = new LoginManager();
$result = $mgr->validateAjaxLogin();

Fakoli::end($result);