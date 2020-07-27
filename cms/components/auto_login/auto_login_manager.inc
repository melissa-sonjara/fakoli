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

Fakoli::using("auto_login");

class AutoLoginManager
{
	static function authenticate()
	{
		$token = $_GET["token"];
		$username = '';
		
		global $user;
		
		$mgr = new UserManager();
		
		try
		{
			$authToken = Query::create(AuthenticationToken, "WHERE token=:token AND enabled=1")
							->bind(":token", $token)
							->executeSingle();

			if ($user)
			{
				if ($user->user_id == $authToken->user_id && Fakoli::isUserActive($user))
				{
					$uri = $authToken->redirect_on_success;
					if (!$uri) $uri = "/index";
				
					redirect($uri);
				}
				else return false;
			}
				
			$user = $authToken->User();
			
			$mgr = new UserManager();
			$usernameField = $mgr->getUsernameField();
			$username = $user->get($usernameField);
			
			if (!Fakoli::isUserActive($user)) return false;
			
			if ($authToken->ip_address)
			{
				
				$ips = explode(",", $authToken->ip_address);
				$ips = str_replace(".", "\\.", $ips);
				$ips = preg_replace("/(.+)/", "/^$1$/", $ips);
				
				$match = false;
				foreach($ips as $ip)
				{
					trace("Testing IP: ".trim($ip)." vs ". $_SERVER["REMOTE_ADDR"], 3);
					if (preg_match(trim($ip), $_SERVER["REMOTE_ADDR"])) 
					{
						$match = true;
						break;
					}
				}
				
				if (!$match) 
				{
					LoginManager::recordLoginAttempt($username, 'auto', $token, 'failure');
					trace("Unauthorized IP Address: {$_SERVER["REMOTE_ADDR"]}", 1);
					throw new FakoliException("Access Denied");
				}
			}
			
			if ($authToken->single_use)
			{
				$authToken->enabled = false;
			}
			
			$authToken->last_access = now();
			$authToken->save();
			
			session_start();
					
			$_SESSION["user"] = $user;
					
			$mgr->loadDefaultSession($user);
			
			$uri = $authToken->redirect_on_success;
			if (!$uri) $uri = "/index";
			
			LoginManager::recordLoginAttempt($username, 'auto', $token, 'success');
			redirect($uri);
		}
		catch(Exception $e)
		{
			trace("Authentication Failed: ".$e->getMessage(), 1);
			
			LoginManager::recordLoginAttempt($username, 'auto', $token, 'failure');
			throw new FakoliException("Authentication Failed");
		}
	}
	
	static function generateToken($id = 0)
	{
		global $user;
		
		if ($id == 0) $id = $user->user_id;
		return sha1($id . now());
	}
	
	static function createOneTimeToken($url, $ip_addr = null)
	{
		global $user;

		if (!$ip_addr) $ip_addr = Fakoli::getLocalIP();
		
		$token = new AuthenticationToken();
		
		$token->token = AutoLoginManager::generateToken();
		$token->user_id = $user->get($user->getPrimaryKey());
		$token->ip_address = $ip_addr;
		$token->enabled = true;
		$token->single_use = true;
		$token->redirect_on_success = $url;
		
		$token->save();
		
		return $token->token;
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
		
		trace("Component auto_login is deleting objects dependent on user_id {$user_id}", 3);
	
		$token = new AuthenticationToken();
		$token->delete("WHERE user_id={$user_id}");
		
		return $user;
	}
	
	static function upgradeComponent($version)
	{
		$mgr = new AutoLoginUpgradeManager();
		$mgr->upgrade($version);
	}	
}
?>