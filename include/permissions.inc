<?php
/*
 * Created on Apr 12, 2007
 *
 * To change the template for this generated file go to
 * Window - Preferences - PHPeclipse - PHP - Code Templates
 */
require_once realpath(dirname(__FILE__)."/../cms/datamodel/site_user.inc");

$user = null;

session_start();

if (isset($_SESSION["user"]))
{
	if ($_POST["Logout"])
	{
		session_destroy();
		unset($user);
	}
	else
	{
		$user = $_SESSION["user"];
	}
}
else
{

	if ($method == "POST")
	{
		if ($_POST["Login"])
		{	
			$user = querySingle(SiteUser, "WHERE username='{$_POST["username"]}' and active=1");
			
			if ($user && crypt($_POST["password"], $user->password) === $user->password)
			{
				session_start();
				
				$_SESSION["user"] = $user;
			}
			else
			{
				$login_error = "<span style='color: red'>Incorrect user name or password</span><br/>";
			}
		}
	}
}

if (!checkRole($page_role))
{
	redirect("index.php");
}

?>
