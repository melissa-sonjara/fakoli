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

/**
 * Defines the Event class.
 */
class Calendar extends DataItem
{
	var $fields = array("calendar_id"	=>	Number,
						"name"			=>	String,
						"identifier"	=>	String,
						"description"	=>	HTML,
						"owner_id"		=>	Number,
						"read_access"	=>	String,
						"write_access"	=>	String,
						"last_modified"	=>	Timestamp);
	
	var $relations = array("Events"	=>	Event,
						   "Owner"	=>	"",
							"EventTypes" => EventType);
	
	function Events($constraint = "")
	{
		return $this->getRelatedList(Event, "", $constraint);
	}
	
	function EventTypes($constraint = "")
	{
		return $this->crossReference(EventType, CalendarEventTypeXref, $constraint);
	}
	
	function Owner()
	{
		$mgr = new UserManager();
		return $mgr->getUser($this->owner_id);
	}
	
	static function findByIdentifier($identifier)
	{
		$match = Query::create(Calendar, "WHERE identifier=:id")
					  ->bind(":id", $identifier)
					  ->executeSingle();

		return $match;
	}
	
	
	function Calendar()
	{
		$this->table = "calendar";
		$this->primary_key = "calendar_id";
		$this->default_format = "{name}";
		
		$this->DataItem(func_get_args());
	}
}
	
/**@}*/?>