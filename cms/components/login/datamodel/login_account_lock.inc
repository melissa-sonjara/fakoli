<?php
/**\addtogroup login
 * @{
 */


class LoginAccountLock extends DataItem
{
	var $table = "login_account_lock";
	var $primary_key = "lock_id";
	
	var $fields = array("lock_id"		=>	Number,
						"username"		=>	String,
						"lock_date"		=>	DateTime,
						"locked_until"	=>	DateTime,
						"active"		=>	Boolean);
}

/**@}*/?>