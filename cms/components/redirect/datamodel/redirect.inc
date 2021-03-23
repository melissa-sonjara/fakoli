<?php
/**\addtogroup redirect
 * @{
 */


class Redirect extends DataItem
{
	var $table = "redirect";
	var $primary_key = "redirect_id";
	
	var $fields = array("redirect_id"			=>	Number,
						"redirect_from"			=>	String,
						"redirect_to"			=>	String,
						"user_id"				=>	Number,
						"active"				=>	Boolean,
						"override"				=>	Boolean,
						"exemption_flag"		=>	String,
						"exempt_roles"			=>	String,
						"preserve_query_string"	=>	Boolean,
						"last_modified"			=>	Timestamp);
	
	var $relations = array("User"	=>	"");
	
	function User()
	{
		$mgr = new UserManager();
		return $mgr->getUser($this->user_id);
	}
}
						
/**@}*/?>