<?php
/**\addtogroup error_log
 * @{
 */



class ErrorLog extends DataItem
{
	// Fields
	
	var $fields = array("error_log_id"	=>	Number,
						"code"			=>	Number,
						"message"		=>	HTML,
						"file"			=>	String,
						"line"			=>	Number,
						"details"		=>	HTML,
						"user_id"		=>	Number,
						"uri"			=>	String,
						"referer"		=>	String,
						"session"		=>	String,
						"timestamp"		=>	Timestamp);
	
	var $relations = array("User"	=>	 "");
	
	function User()
	{
	
		$mgr = new UserManager();
		return $mgr->getUser($this->user_id);
	}
	
	function getUserFullName()
	{
		$u = $this->User();
		if ($u) { return "$u->first_name $u->last_name"; }
	}
	
	function __construct()
	{
		$this->primary_key = "error_log_id";
		$this->table = "error_log";	
		$this->default_format = "{message}";

		// Patch in the user class, since this can be overridden by the application
		$mgr = new UserManager();
		$this->relations["User"] = $mgr->getUserClass(); 
		
		parent::__construct(func_get_args());
	}
}

/**@}*/?>
