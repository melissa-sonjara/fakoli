<?php

class UserActivity extends DataItem
{
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
		$this->table = "user_activity";
		$this->primary_key = "activity_id";
		
		$mgr = new UserManager();
		$this->relations["User"] = $mgr->getUserClass();
		
		$this->DataItem(func_get_args());
	}
}

class UserActivitySession extends DataItem
{
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
		$this->table = "user_activity_session";
		$this->primary_key = "session_id";
	
		$mgr = new UserManager();
		$this->relations["User"] = $mgr->getUserClass();
	
		$this->DataItem(func_get_args());
	}	
}
	
// view
class ActivitySession extends DataItem
{
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
		// view
		$this->table = "activity_sessions";
		$this->primary_key = "session_id";
		
		$mgr = new UserManager();
		$this->relations["User"] = $mgr->getUserClass();
		
		$this->DataItem(func_get_args());
	}	
}
	