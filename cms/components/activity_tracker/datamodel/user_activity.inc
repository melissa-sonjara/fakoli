<?php
/**\addtogroup activity_tracker
 * @{
 */



class UserActivity extends DataItem
{
	var $table = "user_activity";
	var $primary_key = "activity_id";
		
	// Fields
	
	var $fields = array("activity_id"	=>	Number,
						"user_id"		=>	Number,
						"uri"			=>	String,
						"session_id"	=>	String,
						"method"		=>	String,
						"referer"		=>	String,
						"activity_time"	=>	DateTime,
						"response_time"	=>	Number,
					    "memory_usage"	=>	Number);
	
	var $relations = array("User"	=>	SiteUser);
	
	function User()
	{
		$mgr = new UserManager();
		return $mgr->getUser($this->user_id);
	}
	
	function __construct()
	{
		$mgr = new UserManager();
		$this->relations["User"] = $mgr->getUserClass();
		
		parent::__construct(func_get_args());
	}
}

class UserActivitySession extends DataItem
{
	var $table = "user_activity_session";
	var $primary_key = "session_id";
	
	// Fields
	
	var $fields = array("session_id"			=>	Number,
						"session_identifier"	=>	String,
						"user_id"				=>	Number,
						"session_start"			=>	DateTime,
						"session_end"			=>	DateTime,
						"request_count"			=>	Number,
						"feedback_count"		=>	Number,
						"action_count"			=>	Number,
						"page_views"			=>	Number,
						"ip_address"			=>	String,
						"user_agent"			=>	String);

	var $relations = array("User"	=>	SiteUser);
	
	function User()
	{
		$mgr = new UserManager();
		return $mgr->getUser($this->user_id);
	}

	function __construct()
	{
		$mgr = new UserManager();
		$this->relations["User"] = $mgr->getUserClass();
	
		parent::__construct(func_get_args());
	}	
	
	static function fromIdentifier($identifier)
	{
		return Query::create(UserActivitySession, "WHERE session_identifier=:i")
					->bind(":i", $identifier)
					->executeSingle();
	}
	
	// We have an issue where multiple UserActivitySessions have been created for the same identifier.
	// This method helps work around that in the error log.
	
	static function getSessions($identifier)
	{
		return Query::create(UserActivitySession, "WHERE session_identifier=:i")
					->bind(":i", $identifier)
					->execute();
	}
	
}
	
// view
class ActivitySession extends DataItem
{
	// view

	var	$table = "activity_sessions";
	var $primary_key = "session_id";
	
	// Fields
	
	var $fields = array("session_id"	=>	String,
						"page_views"	=>	Number,
						"session_start"	=>	DateTime,
						"session_end"	=>	DateTime,
						"user_id"		=>	Number,
						"num_feedback"	=>	Number);
	
	var $relations = array("User"		=>	SiteUser,
						   "Activity"	=>	UserActivity);
	
	function User()
	{
		$mgr = new UserManager();
		return $mgr->getUser($this->user_id);
	}

	function Activity($constraint = "")
	{
		return $this->getRelatedList(UserActivity, "", $constraint);
	}

	function __construct()
	{
		$mgr = new UserManager();
		$this->relations["User"] = $mgr->getUserClass();
		
		parent::__construct(func_get_args());
	}	
}
	
/**@}*/?>