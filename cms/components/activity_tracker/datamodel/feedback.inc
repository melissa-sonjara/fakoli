<?php

class UserFeedback extends DataItem
{
	// Fields
	
	var $fields = array("feedback_id"	=>	Number,
						"user_id"		=>	Number,
						"session"		=>	String,
						"feedback"		=>	Text,
						"created_date"	=>	DateTime);
	
	// Relations
	
	var $relations = array("User"	=>	SiteUser);
	
	function User()
	{
		return $this->getRelated(SiteUser);
	}
	
	
	function UserFeedback()
	{
		$this->table = "feedback";
		$this->primary_key = "feedback_id";
		
		$this->DataItem(func_get_args());
	}
}

?>