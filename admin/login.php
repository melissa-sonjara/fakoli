<?php

require_once "../include/config.inc";
require_once "../cms/datamodel/site_user.inc";

$hideMenu = true;

if ($method == "POST")
{
	
	$user = querySingle(SiteUser, "WHERE username='{$_POST["username"]}' and active=1 and role='admin'");
	
	if ($user && crypt($_POST["password"], $user->password) === $user->password)
	{
		session_start();
		
		$_SESSION["user"] = $user;
		
		redirect("index.php");
	}
	else
	{
		$msg = "Incorrect user name or password";
	}
}

$title = "Website Administration";

include_once admin_begin_page;
?>
<br><br>
<form method="POST" action="login.php">
<div align="center" style="width:100%">
<table class="form" width="300">
 <tr>
  <th colspan="2">Log in to Administration</th>
 </tr>
 <tr>
  <td colspan="2">
<?
	if ($msg)
	{
?>
   <span style="color: #880000; font-weight: bold"><? echo $msg ?></span>
<?
	}
?>&nbsp;</td>
 </tr>
 <tr>
  <td style="text-align: right"><b>Username:</b></td>
  <td><input type="text" name="username" width="40"></td>
 </tr>
 <tr>
  <td style="text-align: right"><b>Password:</b></td>
  <td><input type="password" name="password" width="40"></td>
 </tr>
 <tr>
  <td colspan="2" style="text-align: center"><br><input type="submit" class="button" name="submit" value="Login"></td>
  </tr>
</table>
</div>
</form>
<br><br><br><br><br><br><br><br><br><br>
<?
include_once admin_end_page;
?>