<?php
/**\addtogroup open_data
 * @{
 */



class OpenData extends DataItem
{
	var $primary_key = "open_data_id";
	var $table = "open_data";

	var $fields = array("open_data_id"		=>	'Number',
						"class"				=>	'String',
						"field"				=>	'String',
						"field_type"		=>	'String',
						"open_data_level"	=>	'String',
						"redacted_field"	=>	'Boolean',
						"redacted_record"	=>  'Boolean',
						"private"			=>	'Boolean',
						"protected"			=>  'Boolean',
						"source"			=>	'String',
						"data_standard"		=>	'String',
						"description"		=>	'HTML',
						"ignored"			=>  'Boolean'
	);
	
	
	static $open_data_level = array("Public"			=>	"Public",
									"Internal"			=>	"Internal",
									"Admin"				=> 	"Admin"	);

}
/**@}*/?>
