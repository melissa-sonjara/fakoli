<?php

class ContextHelp extends DataItem
{
	// Fields
	var $fields = array("help_key"		=>	Number,
						"class_name"	=>	String,
						"field"			=>	String,
						"help"			=>	HTML,
						"criteria"		=>	HTML,
						"title"			=>	String);
	
	// No Relations
	
	var $relations = array();
	
	
	function ContextHelp()
	{
		$this->primary_key = "help_key";
		$this->table = "context_help";
		
		$this->DataItem(func_get_args());
	}
	
}
?>