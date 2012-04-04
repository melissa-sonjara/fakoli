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
		
		global $user;
		
		$mgr = new UserManager();
		if ($user->user_id) redirect("/index");
		
		try
		{
			$authToken = Query::create(AuthenticationToken, "WHERE token=:token AND enabled=1")
							->bind(":token", $token)
							->executeSingle();
							
			$user = $authToken->User();
		
			if (!$user->active) return false;
			
			if ($authToken->ip_address)
			{
				
				$ips = explode(",", $authToken->ip_address);
				$ips = str_replace(".", "\\.", $ips);
				$ips = preg_replace("/(.+)/", "/^$1$/", $ips);
				
				$match = false;
				foreach($ips as $ip)
				{
					if (preg_match(trim($ip), $_SERVER["REMOTE_ADDR"])) 
					{
						$match = true;
						break;
					}
				}
				
				if (!$match) 
				{
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
			
			redirect($uri);
		}
		catch(Exception $e)
		{
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

		if (!$ip_addr) $ip_addr = $_SERVER["SERVER_ADDR"];
		
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
		 
	static function upgradeComponent($version)
	{
		$mgr = new AutoLoginUpgradeManager();
		$mgr->upgrade($version);
	}	
}
?>