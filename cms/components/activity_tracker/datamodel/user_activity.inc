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
						"activity_time"	=>	Timestamp);
	
	var $relations = array("User"	=>	SiteUser);
	
	function User()
	{
		return $this->getRelated(SiteUser);
	}
	
	function UserActivity()
	{
		$this->table = "user_activity";
		$this->primary_key = "activity_id";
		
		$this->DataItem(func_get_args());
	}
}

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
		return $this->getRelated(SiteUser);
	}

	function Activity($constraint = "")
	{
		return $this->getRelatedList(UserActivity, "", $constraint);
	}

	function ActivitySession()
	{
		$this->table = "activity_sessions";
		$this->primary_key = "session_id";
		
		$this->DataItem(func_get_args());
	}	
}
	