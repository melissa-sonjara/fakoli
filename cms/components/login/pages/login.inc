<?php
/**************************************************************

 Copyright (c) 2010 Sonjara, Inc

 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:

 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.

 Except as contained in this notice, the name(s) of the above 
 copyright holders shall not be used in advertising or otherwise 
 to promote the sale, use or other dealings in this Software 
 without prior written authorization.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.

*****************************************************************/

/*
 * Title: /login
 * 
 * Description: Validates the user login.
 * 
 * author: Andy Green for Sonjara, Inc.
 * 
 * date: 2008
 *
 ***************************************************************/

Fakoli::using("login", "text_lookup");

if ($user)
{
	$redirect = Settings::getValue("login", "default_login_redirect");
	if ($redirect)
	{
		redirect($redirect);
	}
}

//optional login text here
echo TextLookup::getText("login_text", null, true);

$enableCookie = Settings::getValue("login", "enable_username_cookie");
$rememberText = Settings::getValue("login", "remember_me_text");
$usernameLabel = Settings::getValue("login", "username_field_label");

if ($enableCookie && !$username)
{
	$username = $_COOKIE["site_user"];
	if ($username) $checked = " checked='checked'";
}

// Login form goes here
?>
<form id="login_form" name='form1' method='POST' action=''>
<table id="login">
<?
	if (LoginManager::$error)
	{
?>
 <tr>
   <td colspan="2"><span class='error'><?echo LoginManager::$error?></span></td>
  </tr>
<?
	}
?>	
  <tr>
   <td><label for="username"><?echo $usernameLabel?>:</label></td>
   <td><input name="username" type="text" value="<?=$username?>"/></td>
  </tr>
  <tr>
   <td><label for="password">Password:</label></td>
   <td><input name="password" type="password"  value="<?=$password?>"/></td>
  </tr>
  <tr>
   <td colspan="2" style="text-align: right">
	<a href="/password_help?username=<?=$username?>&amp;redirect=<?=$redirect?>">Forgotten Password?</a>
   </td>
  </tr>
<?
	if ($enableCookie)
	{
?>
  <tr>
   <td colspan='2'><input type='checkbox' name='remember_me' value='1'<?echo $checked?>/> <?echo $rememberText?></td>
  </tr>
<?
	}
	
	if (LoginManager::$extension)
	{
		LoginManager::$extension->draw();
	}
?>
  <tr>
   <td colspan="2" style="text-align: right">
	<input name="Login" type="submit" value="  Login  " class="button" />
   </td>
  </tr>
</table>
</form>