<?php
/**\addtogroup email
 * @{
 */


/**************************************************************
/*
 * Dummy datamodel to build contact us form. No db table
 * - data is not retreived or stored. 
 * 
 * Janice Gallant for Sonjara, Inc.
 * 
 * 2/14/2011
 *

*****************************************************************/
  

class ForwardEmail extends DataItem
{
	var $fields = array(
				"email_template_id"		=>	'Number',
				"class_name"			=>	'String',
				"name"					=>	'String',
				"sender_email"			=>	'String',
				"recipients"			=>	'Text',
				"subject"				=>	'String',
				"introduction"			=>	'Text',
				"message"				=>	'HTML'				
	);
		
	
	function __construct()
	{
		$this->table = "email";
		$this->primary_key = "email_template_id";
		
		parent::__construct(func_get_args());
	}
}



/**@}*/?>
