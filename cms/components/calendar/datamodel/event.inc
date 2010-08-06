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

/**
 * Defines the Event class.
 */
class Event extends DataItem
{
	var $fields = array("event_id"			=>	Number,
						"title"				=>	String,
						"start_date"		=>	Date,
						"end_date"			=>	Date,
						"location"			=>	String,
						"event_type"		=>	String,
						"description"		=>	HTML,
						"published"			=>	Boolean,
						"last_modified"		=>	Timestamp,
						"composite_class"	=>	String);
						
	// Relations
	
	var $relations = array("Sites"	=>	Site);

	function Sites($constraint = "")
	{
		return $this->crossReference(Site, EventSiteXref, $constraint);
	}
	
	function Event()
	{
		$this->table = "event";
		$this->primary_key = "event_id";
		
		$this->DataItem(func_get_args());
	}
}

class EventSiteXref extends DataItem
{
	var $fields = array("event_site_xref_id"	=>	Number,
						"event_id"				=>	Number,
						"site_id"				=>	Number);
	
	// Relations
	
	var $relations = array(	"Event"	=>	Event,
							"Site"	=>	Site);
	
	function Event()
	{
		return $this->getRelated(Event);
	}
	
	function Site()
	{
		return $this->getRelated(Site);
	}
	
	function EventSiteXref()
	{
		$this->primary_key = "event_site_xref_id";
		$this->table = "event_site_xref";
		
		$this->DataItem(func_get_args());
	}
}
?>
