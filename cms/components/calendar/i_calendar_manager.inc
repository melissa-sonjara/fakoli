<?php

// dummy datamodel


class iCalendarData extends DataItem
{
	var $fields = array(
		"begin_calendar"	=>	String,
		"prodid"			=>	String,
		"version"			=>	String,
		"method"			=>	String,
		"begin_event"		=>	String,
		"organizer"			=>	String,
		"attendee"			=>	String,
		"dtstart"			=>	String,
		"dtend"				=>	String,
		"location"			=>	String,
		"transp"			=>	String,
		"event_status"		=>	String,
		"display_status"	=>	String,
		"uid"				=>	String,
		"sequence"			=>	Number,
		"dtstamp"			=>	String,
		"last_modified"		=>	String,
		"description"		=>	HTML,
		"summary"			=>	String,
		"priority"			=>	Number,
		"class"				=>	String,
		"end_event"			=>	String,
		"end_calendar"		=>	String,
			);
	
	var $fieldAliases = array(
			"begin_calendar"	=>	"BEGIN",
			"begin_event"		=>	"BEGIN",
			"event_status"		=>	"STATUS",
			"display_status"	=> "X-MICROSOFT-CDO-INTENDEDSTATUS",
			"last_modified" 	=> "LAST-MODIFIED",
			"end_event"			=>	"END",
			"end_calendar"		=>	"END",
		);

	function __construct()
	{
		$this->primary_key = "icalendar_uid";
		$this->table = "icalendar";
		
		$this->DataItem(func_get_args());
	}
}


/**
 * Note: we may wish to add someting like this to description:
 *
 The following is a new meeting request:\n\nSubject: test send to
 mac \nOrganizer: "Janice Gallant" <janice@sonjara.com> \n\nTime: Wednesday\
 , March 20\, 2013\, 1:00:00 PM - 2:00:00 PM GMT -05:00 US/Canada Eastern\n \
 nInvitees: janice.gallant@verizon.net \n\n\n*~*~*~*~*~*~*~*~*~*\n\n
 X-ALT-DESC;FMTTYPE=text/html:<html><body><h3>The following is a new meeting
 request:</h3>\n\n<p>\n<table border='0'>\n<tr><th align=left>Subject:</th><t
 d>test send to mac </td></tr>\n<tr><th align=left>Organizer:</th><td>"Janice
 Gallant" &lt\;janice@sonjara.com&gt\; </td></tr>\n</table>\n<p>\n<table bor
 der='0'>\n<tr><th align=left>Time:</th><td>Wednesday\, March 20\, 2013\, 1:0
 0:00 PM - 2:00:00 PM GMT -05:00 US/Canada Eastern\n </td></tr></table>\n<p>\
 n<table border='0'>\n<tr><th align=left>Invitees:</th><td>janice.gallant@ver
 izon.net </td></tr>\n</table>\n<div>*~*~*~*~*~*~*~*~*~*</div><br></body></ht
 ml>
 */

class iCalendarEventManager
{
	var $replyTo;
	var $replyToName;
	var $mailTo;					///< Attendee email address
	var $attendeeName; 				///< optionally include display name of recipient
	var $rsvp = true;				///< organizer wants reply
	var $attendeeStatus;			///< PARSTAT=TENTATIVE, PARSTAT=NEEDS-ACTION
	var $required = false;			///< ROLE=REQ-PARTICIPANT if required to attend
	var $invitees = array(); 		///< optionally show others invited to event
	var $data;						///< iCalendarData obj
	var $aliases = array();			///< field aliases or labels
	
	/**
	 * 
	 * @param Number $id - event id or other unique identifier for the invitation
	 * @param DATE or DATETIME $start
	 * @param DATE or DATETIME $end
	 * @param String $title
	 * @param HTML $description
	 * @param String $location
	 * @param Number $sequence_id - increment sequence id when event is updated
	 */
	function __construct($id, $start, $end, $title, $description = "", $location = "", $sequence_id = 0)
	{
		global $config;
				
		$this->data = new iCalendarData();
		$this->data->summary = $title;
		$this->data->description = $description;
		$this->data->location = $location;

		$uid = $id . $config["http_host"];
		$this->data->uid = $uid;
		
		$this->data->set("sequence", $sequence_id);
		
		$this->data->set("dtstart", $start);
		$this->data->set("dtend", $end);
		
		$today = new DateTime(now());	
		$this->data->set("dtstamp", $today->format('Ymd\THis'));
		
		$this->data->filter = new ExclusionFilter("last_modified");
		// is last modified needed?
		//$this->data->last_modified = $today;
			
		$this->data->set("priority", 5);
		$this->data->set("class", "PUBLIC");
		
		$this->data->set("begin_calendar", "VCALENDAR");
		$this->data->set("prodid", "-//Microsoft Corporation//Outlook 11.0 MIMEDIR//EN");
		$this->data->set("version", "2.0");
		$this->data->set("begin_event", "VEVENT");
		$this->data->set("transp", "OPAQUE");
		$this->data->set("display_status", "BUSY");
		$this->data->set("end_event", "VEVENT");
		$this->data->set("end_calendar", "VCALENDAR");
		$this->data->set("method", "REQUEST");
		$this->data->set("event_status",  "CONFIRMED");
		
		$this->setEventDates($start, $end);
		
		$this->setAliases();
	}
	
	function setAliases()
	{
		$aliases = $this->data->getFieldAliases();
		// Read in default alias definitions from DataItem
		if ($aliases)
		{
			foreach($aliases as $field => $alias)
			{
				$this->aliases[$field] = $alias;
			}
		}	
	}
	
	/**
	 * For all day events:
	 * DTSTART;VALUE=DATE:20100101
	 * DTEND;VALUE=DATE:20100101
	 */
	function setEventDates($start, $end)
	{
		$startObj = new DateTime($start);
		$endObj = new DateTime($end);
		
		if($this->isAllDay($start) && $this->isAllDay($end))
		{
			$start_value = ";VALUE=DATE:" . $startObj->format("Ymd");
			$end_value = ";VALUE=DATE:" . $endObj->format("Ymd");
		}
		else
		{
			$start_value = $startObj->format('Ymd\THis');
			$end_value = $endObj->format('Ymd\THis');
		}
		
		$this->data->set("dtstart", $start_value);
		$this->data->set("dtend", $end_value);
	}
	
	function isAllDay($date)
	{
		if(preg_match("/00:00:00$/", $date))
		{
			return true;
		}
	
		return false;
	}
	
	function setAttachment($path)
	{
		//ATTACH;FMTTYPE=image/jpeg:http://domain.com/images/bastille.jpg
	}
	
	function setOrganizer($replyTo, $replyToName = "")
	{
		$this->replyTo = $replyTo;
		$this->replyToName = $replyToName;
	}
		
	function setAttendee($mailTo)
	{
		$this->mailTo = $mailTo;;
	}
	
	function setAttendeeName($name)
	{
		$this->attendeeName = $name;
	}
		
	/**
	 * Example: ORGANIZER;CN=John Smith:MAILTO:jsmith@host1.com
	 * @param unknown $properties
	 */
	function formatOrganizer()
	{
		if($this->replyToName)
		{
			$organizer .= ";CN={$this->replyToName}";
		}
		
		$organizer .= ":MAILTO:{$this->replyTo}";
		
		return $organizer;
	}
	
	/**
	 * Given all the settings applied to vars, build the
	 * ATTENDEE property to be used in the calendar
	 * invitation.
	 * 
	 * Example: ATTENDEE;ROLE=REQ-PARTICIPANT;PARTSTAT=TENTATIVE;DELEGATED-FROM=
	 * "MAILTO:iamboss@host2.com";CN=Henry Cabot:MAILTO:hcabot@
	 * host2.com
	 */
	function formatAttendee()
	{
		if($this->required)
		{
			$attendee .= ";ROLE=REQ-PARTICIPANT";
		}
		
		if($this->attendeeStatus)
		{
			$attendee .= ";PARTSTAT={$this->attendeeStatus}";
		}
		
		if($this->rsvp)
		{
			$attendee .= ";RSVP=true";
		}
		
		if($this->attendeeName)
		{
			$attendee .= ";CN={$this->attendeeName}";	
		}
			
		$attendee .= ":MAILTO:{$this->mailTo}";
		
		return $attendee;
	}
	
	
	function format()
	{	
		$this->data->organizer = $this->formatOrganizer();
		$this->data->attendee = $this->formatAttendee();
	
		$filter = $this->data->getFilter();
		$fields = $this->data->getFields();
		foreach (array_keys($fields) as $field)
		{
			if ($field != $pk  && !($filter && $filter->isExcluded($field)))
			{
				$label = strtoupper($field);
				if(array_key_exists($field, $this->aliases))
				{
					$label = $this->aliases[$field];
				}
				
				$value = $this->data->$field;
				if(!preg_match("/^;|:/", $value))
				{
					$value = ":". $value;
				}
				$properties[] = $label . $value;
			}
		}
		
		$out = "\r\n";
		$out .= implode("\n", $properties);
		$out .= "\r\n";
		
		return $out;
	}
}