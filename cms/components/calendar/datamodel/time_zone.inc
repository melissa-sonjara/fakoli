<?php
/**\addtogroup calendar
 * @{
 */



class TimeZone extends DataItem
{
	var $fields = array(
		"time_zone_id"				=>	'Number',
		"time_zone_name"			=>	'String',
		"standard_offset"			=>	'Number',
		"daylight_offset"			=>	'Number'
		);
	
	function __construct()
	{
		$this->table = "time_zone";
		$this->primary_key = "time_zone_id";
		
		parent::__construct(func_get_args());
	}
}
/**@}*/?>
