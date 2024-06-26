<?php
/**\addtogroup calendar
 * @{
 */


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

/*
 * Manage component upgrades/updates to Calendar
 * component.
 * 
 * See parent class component/upgrade_manager.inc
 * for instructions on usage.
 *   
 * @author Janice Gallant for Sonjara, Inc.
 * 
 * 6/2/2011
 * 
 */

Fakoli::using("calendar", "component");

class CalendarUpgradeManager extends UpgradeManager
{
	var $updateMap = array(
			"1.0"		=>	"sql:Initial Installation",
			"1.1"		=>	"sql:Add the table event_type and calendar_event_type_xref",
			"1.2"    	=>	"sql: insert data into the event_type table",
			"1.3"		=>	"sql:Add table event_invitation; add event_invitation_id to event table; change event start and end dates to be datetime",
			"1.4"		=>	"sql:Add field allow_access to event table",
			"1.5"		=>	"sql:Add time_zone table and US time zone data; add field time_zone_id to event table",
			"1.6"		=>	"sql:Add column owner_id to event table",
			"1.7"		=>	"sql:Add indexes",
			"1.8"		=>	"sql:Added event_comment_xref table"
	);
	
	function CalendarUpgradeManager()
	{
		$this->component_name = "calendar";

		// sets the sqlFile name according to default
		parent::UpgradeManager();		
	}
}
/**@}*/?>