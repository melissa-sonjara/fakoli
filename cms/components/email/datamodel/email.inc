<?php
/**************************************************************
/*
 * Dummy datamodel to build contact us form. No db table
 * - data is not retreived or stored. 
 * 
 * Reshma Joshi for Sonjara, Inc.
 * 
 * 2/14/2011
 *

*****************************************************************/
  

class Email extends DataItem
{
	var $fields = array(
				"email_id"		=>	Number,
				"sender_name"					=>	String,
				"sender_email"			=>	String,
				"recipient"			=>	Text,
				"subject"				=>	String,
				"message"				=>	Text,
				
	);
		
	
	function Email()
	{
		$this->table = "email";
		$this->primary_key = "email_id";
		
		$this->DataItem(func_get_args());
	}
}


