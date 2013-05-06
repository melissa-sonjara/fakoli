<?php

class UserFeedback extends DataItem
{
	// Fields
	
	var $fields = array("feedback_id"	=>	Number,
						"user_id"		=>	Number,
						"session"		=>	String,
						"feedback"		=>	Text,
						"referer"		=>	String,
						"created_date"	=>	DateTime);
	
	// Relations
	
	var $relations = array("User"	=>	"");
	
	function User()
	{		
		$mgr = new UserManager();
		return $mgr->getUser($this->user_id);
	}
	
	
	function UserFeedback()
	{
		$this->table = "feedback";
		$this->primary_key = "feedback_id";

		// Patch in the user class, since this can be overridden by the application
		$mgr = new UserManager();
		$this->relations["User"] = $mgr->getUserClass(); 	
		
		$this->DataItem(func_get_args());
	}
}

?>