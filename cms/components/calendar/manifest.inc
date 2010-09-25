<?php

class CalendarManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "calendar";
		$component->description = "Basic Calendar display and handling for Fakoli/CMS components";
		$component->author = "Andy Green";
		$component->version = "1.0";
		$component->priority = 4;
		$component->enabled = true;
		
		return $component;
	}
	
	static function getAdminMenu()
	{
		return array
		(
			"Collaboration" => 

				array("Calendars"	=>	array("page"	=>	"/admin/calendars",
					  "role"	=>	"admin",
					  "weight"	=>	2))
		);
	}	

	static function subscribeToEvents()
	{
		return array("ResolveIdentifier" => array(CalendarManager, displayCalendar),
					 "EnumerateItems"	 => array(CalendarManager, enumerateItems));
	}
}

?>