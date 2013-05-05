<?php
/**************************************************************

 Copyright (c) 2010 Sonjara, Inc

 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:

 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.

 Except as contained in this notice, the name(s) of the above 
 copyright holders shall not be used in advertising or otherwise 
 to promote the sale, use or other dealings in this Software 
 without prior written authorization.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.

*****************************************************************/

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
	
	// Allow anonymous access to view event details
	var $allow_sessionless_handlers = array("event_details", "event_list"); 
	
	static function getStyles()
	{
		return array("/components/calendar/css/calendar.css");
	}
	
	static function getScripts()
	{
		return array("/components/calendar/js/calendar.js");
	}
	
	static function getAdminMenu()
	{
		return array
		(
			"Collaboration" => array(
					"Calendars"	=>	array("page"	=>	"/admin/calendars",
					 							   "role"	=>	"admin,editor",
					 							   "weight"	=>	2),
					
					"Calendar Events"	=>	array("page"	=>	"/admin/events",
							"role"	=>	"admin,editor",
							"weight"	=>	3),
					
					"Time Zones"	=>	array("page"	=>	"/admin/time_zones",
							"role"	=>	"admin,editor",
							"weight"	=>	5)
					
					),
					
				
			"Classification" => array("Event Types"	=>	array("page"	=>	"/admin/event_types",
					 							      "role"	=>	"admin",
					  								  "weight"	=>	10)
												     )
		);
	}

	static function subscribeToEvents()
	{
		return array("ResolveIdentifier" => array(CalendarManager, displayCalendar),
					 "EnumerateItems"	 => array(CalendarManager, enumerateItems),
					 "EnumerateContextHelpClasses" => array(CalendarManager, enumerateContextHelpClasses),		
					 "upgradeComponent"		=>	array(CalendarManager, upgradeComponent),
					 "calendarGetHandler"      =>    array(CalendarManager, calendarGetHandler),
					 "RegisterTaxonomyClasses"	=>	array(CalendarManager, registerTaxonomyClasses),
					 "ComponentScanComplete" => array(CalendarManager, setDefaults),
					"DeleteUser"			=>	array(CalendarManager, deleteUser),
		);
	}
}

?>