<?php
/*
 * Created on Apr 12, 2007
 *
 * To change the template for this generated file go to
 * Window - Preferences - PHPeclipse - PHP - Code Templates
 */
require_once realpath(dirname(__FILE__)."/../datamodel/person.inc");

$user = null;



session_start();

if ($_COOKIE["_SJPJ"] && !isset($_SESSION["user"]))
{
	list($id, $hash) = explode("!", $_COOKIE["_SJPJ"]);
	$u = new Person($id);
	$h = sha1($u->password);
	if ($hash == $h)
	{
		$_SESSION["user"] = $u;
	}
}
		
if (isset($_SESSION["user"]))
{
	// Set session user record
	$user = $_SESSION["user"];

	// Set other session variables
	
	$portfoliokey = $_SESSION['portfoliokey'];
	$partners = $_SESSION['partners']; //to know how to show the page
	$access = $_SESSION['access']; // to know whether to show restricted choices on tab
	
}
else
{
	// Handle automatic login redirects
	
	if ($_SERVER['PHP_SELF'] != "/portfolio/login.php")	
	{
		$_SESSION["login_redirect"] = $_SERVER['PHP_SELF'].$_SERVER['QUERY_STRING'];
	}
	redirect("login.php");
}

if (!checkRole($page_role))
{
	redirect("index.php");
}

$link = connect_db();
?>
