<?php
/**\addtogroup sharing
 * @{
 */


class ShareToken extends DataItem
{
	var $table = "share_token";
	var $primary_key = "share_id";
	
	var $fields = array("share_id"				=>	'Number',
						"token"					=>	'String',
						"item_type"				=>	'String',
						"item_key"				=>	'Number',
						"active"				=>	'Boolean',
						"user_id"				=>	'Number',
						"created_date"			=>	'DateTime',
						"last_updated"			=>	'DateTime',
						"last_updated_by_id"	=>	'Number'
	);
	
	var $relations = array(	"User"		=>	'SiteUser',
							"Accesses"	=>	'ShareAccess');
	
	function User() { return $this->getRelated('SiteUser'); }
	function Accesses($constraint = "ORDER BY access_timestamp")
	{
		return $this->getRelatedList('ShareAccess', "", $constraint);
	}
	
	function getTitle()
	{
		$handler = SharingManager::getShareHandler($this->item_type);
		return $handler->getTitle($this->item_key);
	}
	
	function countViews()
	{
		return Query::create('ShareAccess', "WHERE share_id=:s")
					->bind(":s", $this->share_id)
					->count();
	}
	
	static function getShare($token)
	{
		if (!checkPlainGUID($token))
		{
			throw new FakoliException("Invalid Token");
		}
		
		try
		{
			return Query::create('ShareToken', "WHERE token=:t")
						->bind(":t", $token)
						->executeSingle();
		}
		catch(DataNotFoundException $e)
		{
			throw new FakoliException("Token not found");
		}
	}
	
	static function forObject($obj)
	{
		$type = get_class($obj);
		$key = $obj->getPrimaryKeyValue();
		
		$matches = Query::create('ShareToken', "WHERE item_type=:t AND item_key=:k")
						->bind(":t", $type, ":k", $key)
						->execute();
		
		return count($matches) > 0 ? $matches[0] : null;
	}
}


class ShareAccess extends DataItem
{
	var $table = "share_access";
	var $primary_key = "access_id";
	
	var $fields = array("access_id"			=>	'Number',
						"share_id"			=>	'Number',
						"user_id"			=>	'Number',
						"ip_address"		=>	'String',
						"access_timestamp"	=>	'DateTime');
	
	var $relations = array(	"ShareToken"	=>	"ShareToken",
							"User"			=>	"SiteUser");
	
	function __construct()
	{
		return $this->getRelated('ShareToken');
	}
	
	function User()
	{
		return $this->getRelated('SiteUser');
	}
	
	function getFullName()
	{
		if (!$this->user_id) return "Anonymous";
		return $this->User()->getFullName();
	}
}
/**@}*/?>
