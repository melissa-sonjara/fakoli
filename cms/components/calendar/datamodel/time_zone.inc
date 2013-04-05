<?php

class TimeZone extends DataItem
{
	var $fields = array(
		"time_zone_id"				=>	Number,
		"time_zone_name"			=>	String,
		"standard_offset"			=>	Number,
		"daylight_offset"			=>	Number
		);
	
	function TimeZone()
	{
		$this->table = "time_zone";
		$this->primary_key = "time_zone_id";
		
		$this->DataItem(func_get_args());
	}
}