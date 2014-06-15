<?php
class LoginAttempt extends DataItem
{
	var $table = "login_attempt";
	var $primary_key = "login_attempt_id";
	
	var $fields = array("login_attempt_id"	=>	Number,
						"username"			=>	String,
						"ip_address"		=>	String,
						"date_received"		=>	DateTime,
						"login_mode"		=>	String,
						"login_token"		=>	String,
						"result"			=>	String);
}