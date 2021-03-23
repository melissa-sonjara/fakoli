<?php
/**\addtogroup developer_tools
 * @{
 */


class TableInformation extends DataItem
{
	var $table = "INFORMATION_SCHEMA.tables";
	var $primary_key = "TABLE_NAME";
	
	var $fields = array("TABLE_SCHEMA"		=>	String,
						"TABLE_NAME"		=>	String,
						"TABLE_TYPE"		=>	String,
						"ENGINE"			=>	String,
						"TABLE_ROWS"		=>	Number,
						"TABLE_COLLATION"	=>	String);
	
	function save()
	{
		throw new FakoliException("READ ONLY");
	}
	
	function insert()
	{
		throw new FakoliException("READ ONLY");
	}
	
	function update()
	{
		throw new FakoliException("READ ONLY");
	}
	
}
/**@}*/?>