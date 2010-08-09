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
 * Description: Validates the SiteUser login.
 * 
 * author: Andy Green for Sonjara, Inc.
 * 
 * date: 2008
 *
 ***************************************************************/

Fakoli::using("login");

if ($method == "POST")
{
	$username = mysql_escape_string($_POST['username']); //for logging in
	$password = mysql_escape_string($_POST['password']); //for logging in 
	
	$rtn = LoginManager::validateLogin($username, $password, $page);
}

$title = "Web Site Administration";

// Login form goes here
?>
<form name='form1' method='POST' action=''>
<table id="login">
 <tr>
   <td colspan="2">
   <?
	if ($rtn == invalid_username)
		print "<p style=\"padding:20px 10px 0 10px;\"><font color=\"red\">We can not find the username address you provided in the system.<br />Use the link to the right to create a new account.<br />If you need help, please use the 'email us' link below.</font></p>";
	if ($rtn == invalid_password)
		print "<p style=\"padding:20px 10px 0 40px;\"><font color=\"red\">The password you provided is not the same as the one<br />associated with this email address in the system.<br /><b>******NOTE:</b> Passwords are case specific.<br />(example: password and Password are different)</font></p>";
	?>
   </td>
  </tr>
  <tr>
   <td><label for="username">Username:</label></td>
   <td><input name="username" type="text" value="<?=$username?>"/></td>
  </tr>
  <tr>
   <td><label for="password">Password:</label></td>
   <td><input name="password" type="password"  value="<?=$password?>"/></td>
  </tr>
  <tr>
   <td colspan="2" style="text-align: right">
	<a href="/admin/password_help?username=<?=$username?>&amp;redirect=<?=$redirect?>">Forgotten Password?</a>
   </td>
  </tr>
  <tr>
   <td colspan="2" style="text-align: right">
	<input name="Submit" type="submit" value="  Login  " class="button" />
   </td>
  </tr>
</table>
</form>
