<?php
Fakoli::using("auto_login");

class AutoLoginManager
{
	static function authenticate()
	{
		$token = $_GET["token"];
		
		global $user;
		
		$mgr = new UserManager();
		if ($user->user_id) return true;
		
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
				
				if (!$match) return false;
			}
			
			session_start();
					
			$_SESSION["user"] = $user;
					
			$mgr->loadDefaultSession($user);
		}
		catch(Exception $e)
		{
			echo "Authentication Failed";
			return false;
		}
		
		return true;
	}
}
?>