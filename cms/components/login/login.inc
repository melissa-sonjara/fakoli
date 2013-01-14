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
define("invalid_username", 3); // 0 matches found 
define("invalid_password", 4); 

Fakoli::using("user", "email", "settings");

class LoginManager
{
	static $error;
	
	static function emailToken($siteUser)
	{
		global $config;
		
		$pk = $siteUser->getPrimaryKey();
		$id = $siteUser->get($pk);
		
		// clean out old token; only allowed one
		$lt = new LoginToken();
		$lt->delete("WHERE user_id=$id");
				
		$token = LoginToken::generateToken($id);
		
		$mgr = new UserManager();
		
		if(is_callable(array($mgr, emailToken)))
			return $mgr->emailToken($siteUser);
		
		$subject = $config["sitename"];
		
		$userNameField = $mgr->getUsernameField();
		$userNameField = $siteUser->hasField($userNameField) ? $userNameField : $mgr->getEmailField();
		
		$message = "This is a message from {$config["sitename"]}. ";
		$message .= "A temporary password has been created for your account:\n\n";
		$message .= "Username = " . $siteUser->$userNameField ."\n";
		$message .= "Password = " . $token->token . "\n\n";
		$message .= "This password is valid for one login ONLY and expires after ";
		$message .= $token->format("{expire_date} ");
		$message .= "Once you have logged in, you must reset your password. "; 
		$message .=  "Please note that the password is CASE SENSITIVE.";
		$message = wordwrap($message, 80);
		
		$emailFrom = Settings::getValue("email", "email_from");
		
		$headers = "From: $emailFrom\r\n";
		$headers .= "Return-Path: ". $emailFrom . "\r\n";
		$headers .= "Reply-To: ". $emailFrom . "\r\n";	
		$headers .= "Content-type: text;\r\n";
		$headers .= "Mime-Version: 1.0\r\n";
		
		$msg = new Mail($emailFrom, $siteUser->email, $subject, $message, $headers);
		return $msg->send();
	}


	function validateLogin()
	{
		trace("LoginManager validatLogin", 3);
		global $user;
		
		$mgr = new UserManager();

		$username = $_POST[$mgr->getUsernameField()];
		$password = $_POST["password"];
		
		trace("LoginManager validatLogin username $username", 3);
		// JDG - guard against data not found exception
		if(!$username OR !$password)
			return LoginManager::$error = "Incorrect user name or password.";			
	
		$user = $mgr->validatePassword($username, $password);

		if ($user)
		{
			session_start();
			
			$_SESSION["user"] = $user;
			

			trace("USER: ".serialize($user), 1);
			$mgr->loadDefaultSession($user);
			$mgr->loginRedirect($user);
		}
		else
		{
			$user = $this->validateTokenLogin($username, $password);
			
			if ($user)
			{
				$_SESSION["user"] = $user;
				$mgr->loadDefaultSession($user);
				redirect($mgr->getPasswordResetPage());
			}
			else
			{	
				LoginManager::$error = "Incorrect user name or password.";
			}
		}
	}

	function validateAjaxLogin()
	{
		global $user;
		
		$mgr = new UserManager();

		$username = $_POST[$mgr->getUsernameField()];
		$password = $_POST["password"];
		
		// JDG - guard against data not found exception
		if(!$username OR !$password)
		{
			LoginManager::$error = "Incorrect user name or password.";
			return "ERROR|".LoginManager::$error;
		}			
	
		$user = $mgr->validatePassword($username, $password);

		if ($user)
		{
			session_start();
			
			$_SESSION["user"] = $user;
			
			$mgr->loadDefaultSession($user);
			return "OK|".$mgr->getLoginRedirect($user);
		}
		else
		{
			$user = $this->validateTokenLogin($username, $password);
			
			if ($user)
			{
				$_SESSION["user"] = $user;
				$mgr->loadDefaultSession($user);
				return "OK|".$mgr->getPasswordResetPage();
			}
			else
			{	
				LoginManager::$error = "Incorrect user name or password.";
				return "ERROR|".LoginManager::$error;
			}
		}
	}
	

	/*
	 * JDG 2/18/20. If the password does not match, check if the login was
	 * a valid temporary token login
	 */
	function validateTokenLogin($username, $password)
	{
		try
		{		
			$mgr = new UserManager();
			$user = $mgr->getUserByName($username);
			if (!$user) return null;
			
			$token = Query::create(LoginToken, "WHERE user_id=:id")
							->bind(":id", $user->get($user->primary_key))
							->executeSingle();
							
			$today = date('Y-m-d');
				
			if (isset($token) AND $token->token == $password AND $token->expire_date >= $today)
			{
				$token->delete();
				return $user;
			}
			else
			{
				return null;
			}
		}
		catch(Exception $e)
		{
			return null;
		}
	}

	/**
	 * Store the page from which a user has been redirected when prompted 
	 * to login or create an account.
	 */
	function storeRedirectPage()
	{
		global $_SESSION;
	
		$page = $_SERVER["REQUEST_URI"];
		$_SESSION["login_redirect"] = $page;
		trace($page, 3);
	}
	
	/**
	 * If the user was on a page in the system when they were sent to login,
	 * then redirect back to that from page.
	 */
	function redirectToFromPage()
	{
		global $_SESSION;
	
		$page = $_SESSION["login_redirect"];
		unset($_SESSION["login_redirect"]);
	
		$l_user = $_SESSION["user"];
		
		if (!$page || startsWith($page, "components/login"))
			$page = "index.php";	
	
		redirect($page);
	}
	
	/**
	 * Redirect the user to the login page. If the user is logged
	 * in with a mobile device then redirect to the mobile login page
	 * if one is defined.
	 */
	function redirectToLogin()
	{
		LoginManager::storeRedirectPage();
		
		$isMobile = ComponentManager::fireEvent("CheckUseMobile");
		
		$mobileLogin = Settings::getValue("login", "mobile_login_page");
		$mainLogin = Settings::getValue("login", "main_login_page");
		
		if (!$mainLogin) $mainLogin = "/login";
		
		if ($isMobile && $mobileLogin)
		{
			redirect($mobileLogin);
		}
		else redirect($mainLogin);
	}
	
	function passwordHelp($username = "")
	{
		$mgr = new UserManager();

		$usernameField = $mgr->getUsernameField();
		$emailField = $mgr->getEmailField();
		
		if (!$usernameField || !$emailField)
		{
			throw new FakoliException("Unable to provide password help");
		}
		
		if($username)
		{
			$match = $mgr->getUserByName($username);
			if(!$match AND $emailField)
				$match = $mgr->getUserByEmail($username);
		}
		
		if($match)
		{
			$sent = LoginManager::emailToken($match);
			
			if($sent)
			{
				?>
				<br/>
				<p>A temporary password has been sent to the email address registered for your account. 
				The email might take a few minutes to reach you.</p><br/>
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
			<p><em>We cannot find the username you provided in the system.</em></p>
			<?
			LoginManager::passwordHelpForm($username);
		}
		/*
		 * Let the user enter their email address to request their password
		 */
		else
		{
			LoginManager::passwordHelpForm();	
		}
		
		$login = Settings::getValue("login", "main_login_page");		
?>
<h4><a href="<?php echo $login?>">Return to the login page</a></h4>
<?
	}
	
	static function passwordHelpForm($username = "")
	{
		$mgr = new UserManager();
		$passwordHelpText = $mgr->getPasswordHelpText();
		$userNameLabel = $mgr->getPasswordHelpUserNameLabel();
	?>
	<br/>
	<p><?php echo $passwordHelpText ?></p>
	<form name='form1' method='POST' action='/password_help'>
	<p><label for="username"><?php echo $userNameLabel ?>:&nbsp;</label>
	<input name="username" type="text" value="<?=$username?>"/>
	</p>
	<br/>
	<p><input name="Submit" type="submit" value="  Email Me  " class="button" /></p>
	<br/>
	</form>
	<? 	
	}
	
	static function setDefaults()
	{
		trace("Setting Login defaults", 4);
		
		Settings::setDefaultValue("login", "enable_user_login", true, "Boolean");

		Settings::setDefaultValue("login", "main_login_page", "/login", String, "Login page for desktop users");
		Settings::setDefaultValue("login", "mobile_login_page", "", String, "Login page for mobile users (if blank, the main login page is used)");
	}
	
	static function upgradeComponent($version)
	{
		$mgr = new LoginUpgradeManager();
		$mgr->upgrade($version);
	}	
}