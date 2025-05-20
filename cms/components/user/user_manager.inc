<?php
/**
 * \addtogroup user
 * @{
 */
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

/**
 * Provides the interface to the user model for the application.
 * This implementation uses the default user model as shipped with
 * Fakoli/CMS. If you need to use a different user model you can
 * simply implement your own 'user' component and include it in
 * your application web root to override this one. In that case
 * you must supply a UserManager class that conforms to the interface
 * below.
 * 
 * @author andy
 */
class UserManager
{
	/**
	 * Creates a new UserManager object.
	 */
	function __construct()
	{
	}
	
	/**
	 * Returns the field name of the username in the user model.
	 * @return string
	 */
	function getUsernameField()
	{
		return "username";
	}
	
	/**
	 * Returns the field name of the email field in the user model.
	 * @return string
	 */
	function getEmailField()
	{
		return "email";
	}
	
	/**
	 * Returns the datamodel class name of the user model.
	 * @return string
	 */
	function getUserClass()
	{
		return SiteUser;
	}
	
	/**
	 * Retrieves the user with the specified id
	 * @param $user_id the primary key identifying the user
	 * @return object
	 */
	function getUser($user_id)
	{
		return new SiteUser($user_id);
	}
	
	/**
	 * Return a clause that constrains records on those that
	 * are active.
	 * 
	 * @return string
	 */
	function getActiveClause()
	{
		return "active=true";
	}

	/**
	 * Determine if the specified user account is active
	 * @param SiteUser $user
	 */
	function isActive($user)
	{
		return $user->active;
	}
	
	function getUserByName($username)
	{
		$usernameField = $this->getUsernameField();
		
		$matches = Query::create(SiteUser, "WHERE $usernameField=:username")
				->bind(":username", $username)
				->execute();
		
		return (count($matches) > 0) ? $matches[0] : null;
	}
	
	
	function getUserByEmail($email)
	{
		$emailField = $this->getEmailField();
		
		$matches = Query::create(SiteUser, "WHERE $emailField=:email")
				->bind(":email", $email)
				->execute();
		
		return (count($matches) > 0) ? $matches[0] : null;
	}

	/**
	 * Return the user's full name
	 * @param string $user the user object to be queried
	 */
	function getUserFullName($user)
	{
		return $user->getFullName();
	}
	
	/**
	 * Validates the user's username and password, returning the matching user.
	 * @param $username the user's username
	 * @param $password the supplied password
	 * @return object the matching user if password is valid, null otherwise
	 */
	function validatePassword($username, $password)
	{
		$user = querySingle(SiteUser, "WHERE username='{$username}' and active=1");
			
		if ($user && crypt($_POST["password"], $user->password) === $user->password)
		{
			return $user;
		}
		else
		{
			return null;
		}
	}
	
	/**
	 * Load any supporting information into the user's session after a valid login.
	 * @param $user the user object
	 */
	function loadDefaultSession($user)
	{
	}
	
	/**
	 * Redirect the user after a valid login.
	 * @param $user the user object
	 */
	function loginRedirect($user)
	{
		$page = $this->getLoginRedirect($user);
		redirect($page);
	}
	/**
	 * Redirect the user after a valid login.
	 * @param $user the user object
	 */
	function getLoginRedirect($user)
	{
		global $isAdmin;
		global $_SESSION;
	
		$page = $_SESSION["login_redirect"];
		unset($_SESSION["login_redirect"]);
	
		if (!$page || startsWith($page, "components/login"))
		{
			$page = ($isAdmin) ? "/admin/index" : SiteRole::getHomePage($user);
		}
	
		return $page;
	}

	/**
	 * Returns the identifier of the password reset page.
	 * @return string the identifier of the password reset page.
	 */
	function getPasswordResetPage()
	{
		return "/password_reset";
	}
	
	function getPasswordHelpText()
	{
		return "If you have an account on this system, please enter your user name or your user email address and we will email a temporary password to you.";
	}
	
	function getPasswordHelpUserNameLabel()
	{
		return "User name or email";	
	}
	
	static function deleteRole($del_role)
	{
		$constraint = "WHERE role like '%$del_role%'";
		
		$mgr = new UserManager();
		$class = $mgr->getUserClass();
		$users = Query::create($class, $constraint)->execute();
		
		RoleManager::deleteRoleFromString($users, $del_role);
		
		return $del_role;
	}		
	
	static function onInitialize()
	{
		global $user;
		if ($user)
		{
			MenuManager::setDisplayFlag('logged_in', true);
		}
		else
		{
			MenuManager::setDisplayFlag('logged_in', false);
		}
	}
	
	static function upgradeComponent($version)
	{
		$mgr = new UserUpgradeManager();
		$mgr->upgrade($version);
	}	
}
/**@}*/?>
