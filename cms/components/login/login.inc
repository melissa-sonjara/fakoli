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

/**
 * Defines the interface for applications to provide extensions to the default login form.
 * These extensions can (but are not required to) provide additional authentication factors
 * that are validated during the login request.
 * @author Andy Green
 */
interface LoginExtension
{
	/**
	 * Draw the user interface for the login extension
	 */
	function draw();
	
	/**
	 * Validates the login request for the specified user.
	 * @param string $user
	 * @return boolean
	 */
	function validate($user);
	
	/**
	 * Returns the error message describing the validation failure 
	 * that occurred in the last called to validate().
	 */
	function getError();
}

/**
 * LoginManager provides the authentication workflow for logging users into the system. 
 * Validation is generally deferred to the user component, which applications can override
 * to provide application-specific login validation.
 * @author Andy Green
 */
class LoginManager
{
	static $error;
	static $extension = null;
	
	/**
	 * Registers an extension to the application's login form
	 * @param LoginExtension $extension
	 */
	static function registerLoginExtension($extension)
	{
		LoginManager::$extension = $extension;
	}
	
	/**
	 * Records a login attempt in the login audit trail. If account locking is enabled,
	 * the account may be locked by this method as a result of a failed authentication.
	 * @param string $username the username of the account that is attempting to log in
	 * @param string $mode the authentication mode ('login', 'token', etc.)
	 * @param string $token supplementary login token (if provided)
	 * @param string $result the result of the authentication attempt ('success', 'failure')
	 */
	static function recordLoginAttempt($username, $mode, $token, $result)
	{
		$attempt = new LoginAttempt();
		$attempt->username = $username;
		$attempt->ip_address = $_SERVER['REMOTE_ADDR'];
		$attempt->login_mode = $mode;
		$attempt->login_token = $token;
		$attempt->result = $result;
		$attempt->date_received = now();
		$attempt->save();
		
		if (Settings::getValue("login", "enable_account_locking"))
		{
			if ($result == "failure")
			{
				$window = Settings::getValue("login", "login_attempt_window");
				
				$from = new DateTime();
				$from->modify("-{$window} minutes");
				$fromStr = $from->format("Y-m-d H:i:s");
				
				$failed = Query::create(LoginAttempt, "WHERE username=:u AND date_received > :f and result='failure'")
							   ->bind(":u", $username, ":f", $fromStr)
							   ->executeValue("COUNT(1)");
				
				if ($failed >= Settings::getValue("login", "maximum_failed_attempts"))
				{
					LoginManager::lockAccount($username);
				}
			}
		}
	}
	
	/**
	 * Locks the specified user account
	 * @param string $username the username for the account that is to be locked.
	 */
	static function lockAccount($username)
	{
		$lockFrom = new DateTime();
		$lockTo = new DateTime();
		
		$duration = Settings::getValue("login", "lockout_duration");
		
		$lockTo->modify("+{$duration} minutes");
		
		$lock = new LoginAccountLock();
		$lock->username = $username;
		$lock->lock_date = $lockFrom->format("Y-m-d H:i:s");
		$lock->locked_until = $lockTo->format("Y-m-d H:i:s");
		$lock->active = true;
		$lock->save();
	}
	
	/**
	 * Retrieve any current account lock for the specified user account
	 * @param string $username the user name for the account
	 * @return LoginAccountLock|NULL
	 */
	static function getAccountLock($username)
	{
		try
		{
			$lock = Query::create(LoginAccountLock, "WHERE username=:u AND locked_until > :c AND active=1 ORDER BY locked_until DESC LIMIT 1")
						  ->bind(":u", $username, ":c", now())
						  ->executeSingle();
			
			return $lock;
		}
		catch(DataNotFoundException $e)
		{
			return null;
		}
	}
	
	/**
	 * Emails a temporary login token to the specified user
	 * @param unknown $siteUser
	 */
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
		
		$subject = "Password help for ".$config["sitename"];
		
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
		
		$headers  = "Return-Path: ". $emailFrom . "\r\n";
		$headers .= "Reply-To: ". $emailFrom . "\r\n";	
		$headers .= "Content-type: text;\r\n";
		$headers .= "Mime-Version: 1.0\r\n";
		
		$msg = new Mail($emailFrom, $siteUser->email, $subject, $message, $headers);
		return $msg->send();
	}


	/**
	 * Validate the Login from the login page. The login page may be the
	 * standard Fakoli login page built from the login module in component
	 * login or it may be a custom page. All login forms MUST tag the
	 * username/email input field as "username", regardless of what
	 * the field represents:
	 * <input name='username'...
	 * 
	 * @return string
	 */
	function validateLogin()
	{
		trace("LoginManager validateLogin", 3);
		global $user;

		$mgr = new UserManager();

		$username = $_POST["username"];
		$password = $_POST["password"];
		$remember = checkNumeric($_POST["remember_me"]);

		if (Settings::getValue("login", "enable_account_locking"))
		{
			if (LoginManager::getAccountLock($username))
			{
				return LoginManager::$error = "Access to this account has been temporarily locked";
			}
		}
		
		trace("Fakoli LoginManager validateLogin username $username", 3);
		trace(print_r($_POST, true), 3);
		
		// JDG - guard against data not found exception
		if(!$username OR !$password)
		{
			LoginManager::recordLoginAttempt($username, "login", null, "failure");
			return LoginManager::$error = "Incorrect user name or password.";			
		}
		
		$user = $mgr->validatePassword($username, $password);

		if ($user && LoginManager::$extension)
		{
			if (!LoginManager::$extension->validate($user))
			{
				LoginManager::recordLoginAttempt($username, "login", null, "failure");
				LoginManager::$error = LoginManager::$extension->getError();
				$user = null;
			}
		}
		
		if ($user)
		{
			session_start();
			
			$_SESSION["user"] = $user;

			trace("USER: ".serialize($user), 1);

			if (Settings::getValue("login", "enable_username_cookie") && $remember)
			{
				$expiry = time() + 86400 * Settings::getValue("login", "remember_me_cookie_expiration");
				setcookie("site_user", $username, $expiry);
			}
			else
			{
				if ($_COOKIE["site_user"])
				{
					setcookie("site_user", "", time() - 3600);
				}
			}
				
			LoginManager::recordLoginAttempt($username, "login", null, "success");
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
				LoginManager::recordLoginAttempt($username, "token", null, "success");
				redirect($mgr->getPasswordResetPage());
			}
			else
			{	
				LoginManager::recordLoginAttempt($username, "login", null, "failure");
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

			LoginManager::recordLoginAttempt($username, "ajax", null, "success");
			
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
				LoginManager::recordLoginAttempt($username, "token", null, "success");
				return "OK|".$mgr->getPasswordResetPage();
			}
			else
			{	
				LoginManager::$error = "Incorrect user name or password.";
				LoginManager::recordLoginAttempt($username, "ajax", null, "failure");
				return "ERROR|".LoginManager::$error;
			}
		}
	}
	
	/**
	 * Respond to fired event DeleteUser
	 *
	 * Delete any records in this component that have dependencies on user object.
	 *
	 * @param obj $user - class SiteUser or custom user class
	 */
	static function deleteUser($user)
	{
		$pk = $user->getPrimaryKey();
		$user_id = $user->$pk;
		
		trace("Component login is deleting objects dependent on user_id {$user_id}", 3);
	
		$token = new LoginToken();
		$token->delete("WHERE user_id={$user_id}");
		
		return $user;
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
	static function storeRedirectPage()
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
	static function redirectToFromPage()
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
	static function redirectToLogin()
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
	<form name='form1' method='POST' action='<?php echo Fakoli::$requestURI?>'>
	<p><label for="username"><?php echo $userNameLabel ?>:&nbsp;</label>
	<input name="username" type="text" value="<?=$username?>"/>
	</p>
	<br/>
	<p><input name="Submit" type="submit" value="<?echo Settings::getValue("login", "password_help_submit_label")?>" class="button" /></p>
	<br/>
	</form>
	<? 	
	}
	
	static function setDefaults()
	{
		trace("Setting Login defaults", 4);
		
		Settings::setDefaultValue("login", "enable_user_login", true, "Boolean", "Turns on or off user authentication. When turned off, no users can access the site except for admins", "Access Control");
		Settings::setDefaultValue("login", "enable_username_cookie", false, Boolean, "Enables or disables the 'Remember my username' option on the default login page", "Username Cookie");
		Settings::setDefaultValue("login", "remember_me_text", "Remember my Username", String, "The text to display next to the 'remember me' checkbox on the default login page", "Username Cookie");
		Settings::setDefaultValue("login", "remember_me_cookie_expiration", 15, Number, "The number of days for which the username is to be remembered", "Username Cookie");
		
		Settings::setDefaultValue("login", "username_field_label", "Username", String, "The label for the username field on the login form", "Labels and Text", null, 1);
		Settings::SetDefaultValue("login", "password_help_submit_label", "  Email Me  ", String, "The text for the password help submit button", "Labels and Text", null, 2);
		
		Settings::setDefaultValue("login", "main_login_page", "/login", String, "Login page for desktop users", "Login Redirects");
		Settings::setDefaultValue("login", "mobile_login_page", "", String, "Login page for mobile users (if blank, the main login page is used)", "Login Redirects");
		Settings::setDefaultValue("login", "default_login_redirect", "", String, "The page to which users are redirected when they attempt to access the login page when logged in", "Login Redirects");

		Settings::setDefaultValue("login", "enable_account_locking", false, Boolean, "Turns on or off automatic account locking after a given number of failed login attempts", "Account Locking", null, 1);
		Settings::setDefaultValue("login", "maximum_failed_attempts", 3, Number, "The maximum number of failed attempts before the account is locked", "Account Locking", null, 2);
		Settings::setDefaultValue("login", "login_attempt_window", 20, Number, "The window within which the number of failed login attempts is counted (in minutes)", "Account Locking", null, 3);
		Settings::setDefaultValue("login", "lockout_duration", 60, Number, "The duration for which the account will be locked (in minutes)", "Account Locking", null, 4);
		
		Settings::setDefaultValue("login", "enable_password_complexity_rules", true, Boolean, "", "Password Complexity", null, 1);
		Settings::setDefaultValue("login", "minimum_password_length", 6, Number, "", "Password Complexity", null, 2);
		Settings::setDefaultValue("login", "maximum_password_length", 128, Number, "", "Password Complexity", null, 3);
		Settings::setDefaultValue("login", "require_digits", true, Boolean, "Require at least one numeric character", "Password Complexity", null, 4);
		Settings::setDefaultValue("login", "require_uppercase", true, Boolean, "Require at least one uppercase character", "Password Complexity", null, 5);
		Settings::setDefaultValue("login", "require_special", true, Boolean, "Require at least one special (punctuation or whitespace) character", "Password Complexity", null, 6);
	}
	
	static function registerFieldRenderers()
	{
		global $styles;
	
		if (Settings::getValue("login", "enable_password_complexity_rules"))
		{
			AutoForm::registerFieldRendererClass(Password, PasswordComplexityFieldRenderer);
		}
	}
	
	static function upgradeComponent($version)
	{
		$mgr = new LoginUpgradeManager();
		$mgr->upgrade($version);
	}	
}