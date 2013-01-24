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
 * 
 * Title: password_help.php
 * 
 * Description: This script emails to the user a temporary
 * token login that can be used to login and then reset
 * their password.
 * 
 * If the email  parameter is not valid or not an 
 * email address, then prompt the user for a valid email and also.
 * 
 * author: Janice Gallant for Sonjara, Inc.
 * 
 * date: 4/19/10
 * 
 */
Fakoli::using("login");

$username = $_GET['username'];

$mgr = new UserManager();
$usernameField = $mgr->getUsernameField();
$emailField = $mgr->getEmailField();

if (!$usernameField || !$emailField)
{
	throw new FakoliException("Unable to provide password help");
}

if($username)
{
	$matches = query(SiteUser, "WHERE username='$username'");

	if(count($matches) > 0)
		$match = $matches[0];
}


if($match)
{
	$sent = emailToken($match);
	
	if($sent)
	{
		?>
		<br/>
		<p>Your temporary password has been sent to your email address: <?echo $match->email ?>. The email might
				take a few minutes to reach you.</p><br/>
		<?
	}
	else
	{	
		?>
		<br/>
		<p>Mail could not be sent. Please contact <a href="<? echo $config["email_contact"]?>"><? echo $config["email_name"]?></a> for help.</p><br/>
		<?	
	} 
} // end if match

elseif($username)
{
	?>
	<p><em>We can not find the email address you provided in the system.</em></p>
	<?
	passwordHelpForm($username);
}
/*
 * Let the user enter their email address to request their password
 */
else
{
	passwordHelpForm();	
}
?>
<h4><a href="/login">Return to the login page</a></h4>
<?

function passwordHelpForm($username = "")
{
	?>
	<br/>
	<p>If you have an account on this system, please enter your email username and we will email a temporary password to you.</p>
	<form name='form1' method='GET' action=''>
	<p><label for="email">Email:</label>
	<input name="username" type="text" value="<?=$username?>"/>
	</p>
	<br/>
	<p><input name="Submit" type="submit" value="  Email Me  " class="button" /></p>
	<br/>
	</form>
	<? 	
}

?>