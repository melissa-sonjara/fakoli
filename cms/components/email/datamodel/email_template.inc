<?php



class EmailTemplate extends DataItem
{
	var $fields = array(
				"email_template_id"		=>	Number,
				"name"					=>	String,
				"recipients"			=>	Text,
				"subject"				=>	String,
				"message"				=>	HTML,
				"class_name"			=>	String
	);
		
	
	function EmailTemplate()
	{
		$this->table = "email_template";
		$this->primary_key = "email_template_id";
		
		$this->DataItem(func_get_args());
	}
}


