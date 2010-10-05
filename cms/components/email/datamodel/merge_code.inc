<?php

class MergeCode extends DataItem
{
	var $fields = array(
			"merge_code_id"		=>	Number,
			"name"				=>	String,
			"description"		=>	Text,
			"map"				=>	String,
			"function"			=>	Boolean,
			"class_name"		=>	String
	);

	
	function MergeCode()
	{
		$this->table = "merge_code";
		$this->primary_key = "merge_code_id";
		
		$this->DataItem(func_get_args());		
	}
}



?>