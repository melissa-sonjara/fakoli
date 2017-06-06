<?php
/**
 * EventMapRecord is a pseudo-DataItem used to convert the Event Map
 * entries into a format that can be used with our display controls.
 * @author Andy Green
 */
class EventMapRecord extends DataItem
{
	var $fields = array("component"	=>	String,
						"priority"	=>	Number,
						"handler"	=>	String,
						"exists"	=>	Boolean
	);
	
	/**
	 * Converts an Event Map record entry to a DataItem
	 */
	static function convert($obj)
	{
		$record = new EventMapRecord();
		$record->component = $obj["component"];
		$record->priority = $obj["priority"];
		$record->handler = is_array($obj["handler"]) ? implode("::", $obj["handler"]) : $obj["handler"];
	
		if (is_array($obj["handler"]))
		{
			$record->exists = method_exists($obj["handler"][0], $obj["handler"][1]);
		}
		else
		{
			$record->exists = function_exists($obj["handler"]);
		}
		return $record;
	}
	
	/**
	 * Converts and sorts a full event map
	 * @param array $map
	 * @return array
	 */
	static function convertMap($map)
	{
		$converted = array();
		
		foreach($map as $event => $subscribers)
		{
			if (!isset($converted[$event])) $converted[$event] = array();

			foreach($subscribers as $subscriber)
			{
				$converted[$event][] = EventMapRecord::convert($subscriber);
			}
		}
		
		ksort($converted);
		return $converted;
	}
}
