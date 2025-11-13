<?php
/**\addtogroup api
 * @{
 */



class APIToken extends DataItem
{
	var $table = "api_token";
	var $primary_key = "token_id";
	var $pretty_class_name = "API Token";
	
	var $fields = array("token_id"		=>	'Number',
						"token"			=>	'String',
						"user_id"		=>	'Number',
						"created_date"	=>	'DateTime',
						"expiry_date"	=>	'DateTime',
						"last_access"	=>	'DateTime',
						"active"		=>	'Boolean');
	
	var $related = array("User"	=>	'');
	
	function User()
	{
		$mgr = new UserManager();
		return $mgr->getUser($this->user_id);
	}
	
	function __construct()
	{
		// Patch in the user class, since this can be overridden by the application
		$mgr = new UserManager();
		$this->relations["User"] = $mgr->getUserClass();
		
		parent::__construct(func_get_args());
	}	
	
	static function getUserToken($user_id)
	{
		$tokens = Query::create('APIToken', "WHERE user_id=:u AND active=1 AND (expiry_date IS NULL OR expiry_date > NOW()) ORDER BY created_date DESC LIMIT 1")
					   ->bind(":u", $user_id)
					   ->execute();
		
		if (count($tokens)) return $tokens[0];
		return null;
	}
	
	static function getToken($token)
	{
		$tokens = Query::create('APIToken', "WHERE token=:t AND active=1 AND (expiry_date IS NULL OR expiry_date > NOW()) ORDER BY created_date DESC LIMIT 1")
					   ->bind(":t", $token)
					   ->execute();
		
		if (count($tokens)) return $tokens[0];
		return null;
	}
}

/**@}*/?>
