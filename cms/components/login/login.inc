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
 * Title: login.inc
 * 
 * Description: Functions for validating site user login
 * and providing temporary login tokens.
 * 
 * author: Janice Gallant for Sonjara, Inc.
 * 
 * date: 4/19/10
 * 
 */

 // return codes
define("valid_login", 1);
define("valid_token_login", 2);
define("invalid_username", 3; // 0 matches found 
define("invalid_password", 4); 


function emailToken($siteUser)
{
	global $config;
	
	// clean out old token; only allowed one
	$oldToken = $siteUser->LoginToken();
	if(isset($oldToken))
		$oldToken->delete();
	
	$pk = getPrimaryKey($siteUser);
	$token = LoginToken::generateToken($siteUser->$pk);
	
	$subject = $config["sitename"];
	
	$message = "This is a message from {$config["sitename"]}. ";
	$message .= "A temporary password has been created for your account:\n\n";
	$message .= "Password = " . $token->token . "\n\n";
	$message .= "This password is valid for one login ONLY and expires after ";
	$message .= formatDateShort($token->expire_date) . ". ";
	$message .= "Once you have logged in, you must reset your password. "; 
	$message .=  "Please note that the password is CASE SENSITIVE.";
	$message = wordwrap($message, 100);
	
	$emailFrom = $config["email_from"];
	
	$headers = "From: $emailFrom\r\n";
	$headers .= "Return-Path: ". $emailFrom . "\r\n";
	$headers .= "Reply-To: ". $emailFrom . "\r\n";	
	$headers .= "Content-type: text;\r\n";
	$headers .= "Mime-Version: 1.0\r\n";
	
	$msg = new Mail($emailFrom, $siteUser->email, $subject, $message, $headers);
	return $msg->send();
	
	return $sent;	
}


function validateLogin($username, $password, &$page)
{
	global $_SESSION;
	
	if (isset($_SESSION["user"]))
	{
		if ($username == $_SESSION["user"]->email)
		{
			// Already logged in, redirect to home
			redirect("index.php");
		}
		else
		{
			// Logging in as someone else - log off the previous user before continuing 
			unset($_SESSION["user"]);	
		}
	}
	
	if ($username && $password)
	{	
		$rtn = 0;
		
		$matches = query(Teacher, "WHERE email='$username'");

		trace("Matches: ".count($matches), 3);

		if(count($matches) > 0)
			$match = $matches[0];
	
		if($match)
		{
			$hashedPassword = $match->hashPassword($password);
			if ($hashedPassword == $match->password)
				$rtn = valid_login;
			else
				$rtn = validTokenLogin($match, $password))
		}
		else
			$rtn = invalid_username;
	
		if($rtn == valid_login || $rtn == valid_token_login)
			$_SESSION["user"] = $match;
			
		if($rtn == valid_login)
			redirectToFromPage();
		elseif($rtn == valid_login_token)
		{	// must reset their password
			redirect("myprofile.php?reset=1");
		}
	}	
	return $rtn;	
}



/*
 * JDG 2/18/20. If the password does not match, check if the login was
 * a valid temporary token login
 */
function validTokenLogin($match, $password, &$tokenLogin = false)
{
	$token = $match->LoginToken();
	$today = date('Y-m-d');
		
	if (isset($token) AND $token->token == $password AND $token->expire_date >= $today)
	{
		$tokenLogin = true;
		$token->delete();
		return valid_token_login;
	}
	else
		return invalid_password;
}


/*
 * If the user was on a page in the system when they were sent to login,
 * then redirect back to that from page.
 */
function redirectToFromPage()
{
	global $_SESSION;

	$page = $_SESSION["login_redirect"];
	unset($_SESSION["login_redirect"]);

	$l_user = $_SESSION["user"];
	
	if (!$page || startsWith($page, "components/login.php"))
		$page = "index.php";	

	redirect($page);
}
