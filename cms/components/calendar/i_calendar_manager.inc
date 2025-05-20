<?php
/**\addtogroup calendar
 * @{
 */



/**
 * Dummy datamodel to helper manage the calendar data values.
 * 
 * For most of the fields, the field name converted to upper case
 * is the label that should be ouput to icalendar
 * e.g., "prodid"  becomes "PRODID"
 * 
 * For others, we use fieldAliases to get the label.
 * 
 * Data filtering can be used to exclude one ore fields if desired.
 * 
 * @author janice
 *
 */

class iCalendarData extends DataItem
{
	var $fields = array(
		"begin_calendar"		=>	'String',
		"prodid"				=>	'String',
		"version"				=>	'String',
		"method"				=>	'String',
		"begin_timezone"		=>	'String',
		"tzid"					=>	'String',
		"begin_standard"		=>	'String',
		"standard_start"		=>	'String',
		"standard_offset"		=>	'String',
		"standard_dst_offset"	=>	'String',
		"standard_rrule"		=>	'String',
		"standard_name"			=>	'String',
		"end_standard"			=>	'String',
		"begin_daylight"		=>	'String',
		"daylight_start"		=>	'String',
		"daylight_offset"		=>	'String',
		"daylight_dst_offset"	=>	'String',
		"daylight_rrule"		=>	'String',
		"daylight_name"			=>	'String',
		"end_daylight"			=>	'String',
		"end_timezone"			=>	'String',
		"begin_event"			=>	'String',
		"organizer"				=>	'String',
		"attendee"				=>	'String',
		"dtstart"				=>	'String',
		"dtend"					=>	'String',
		"location"				=>	'String',
		"transp"				=>	'String',
		"event_status"			=>	'String',
		"display_status"		=>	'String',
		"uid"					=>	'String',
		"sequence"				=>	'Number',
		"dtstamp"				=>	'String',
		"last_modified"			=>	'String',
		"description"			=>	'HTML',
		"summary"				=>	'String',
		"priority"				=>	'Number',
		"class"					=>	'String',
		"end_event"				=>	'String',
		"end_calendar"			=>	'String',
			);
	
	var $fieldAliases = array(
			"begin_calendar"		=>	"BEGIN",
			"begin_event"			=>	"BEGIN",
			"event_status"			=>	"STATUS",
			"display_status"		=> "X-MICROSOFT-CDO-INTENDEDSTATUS",
			"last_modified" 		=> "LAST-MODIFIED",
			"end_event"				=>	"END",
			"end_calendar"			=>	"END",
			
			"begin_timezone"		=>	"BEGIN",
			
			"begin_standard"		=>	"BEGIN",
			"standard_start"		=>	"DTSTART",
			"standard_offset"		=>	"TZOFFSETFROM",
			"standard_dst_offset"	=>	"TZOFFSETTO",
			"standard_rrule"		=>	"RRULE",
			"standard_name"			=>	"TZNAME",
			"end_standard"			=>	"END",
			
			"begin_daylight"		=>	"BEGIN",
			"daylight_start"		=>	"DTSTART",
			"daylight_offset"		=>	"TZOFFSETFROM",
			"daylight_dst_offset"	=>	"TZOFFSETTO",
			"daylight_rrule"		=>	"RRULE",
			"daylight_name"			=>	"TZNAME",
			"end_daylight"			=>	"END",
				
			"end_timezone"			=>	"END",
		);

	function __construct()
	{
		$this->primary_key = "icalendar_uid";
		$this->table = "icalendar";
		
		parent::__construct(func_get_args());
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
	var $replyTo;					///< organizer email set in EmailHandler
	var $replyToName;				///< organizer name set in EmailHandler
	var $mailTo;					///< Attendee email address
	var $attendeeName; 				///< optionally include display name of recipient
	var $rsvp = true;				///< organizer wants reply
	var $attendeeStatus;			///< PARSTAT=TENTATIVE, PARSTAT=NEEDS-ACTION
	var $required = false;			///< ROLE=REQ-PARTICIPANT if required to attend
	var $invitees = array(); 		///< optionally show others invited to event
	var $data;						///< iCalendarData obj
	var $aliases = array();			///< field aliases or labels
	
	/**
	 * The iCalendarManager obj is typically instantiated through the EventHandler
	 * for an event. E.g., the StandardEventHandler class can all formatiCalendar
	 * to instantiate an iCalendarManager obj by providing its event details
	 * such as start_date, end_date, location, etc.
	 * 
	 * After the obj is created, the calling code can set other configurable values
	 * such as attendee name, if known. It can also override default values set
	 * in the constructor, such as method.
	 * 
	 * @param Number $id - event id or other unique identifier for the invitation
	 * @param DATE or DATETIME $start
	 * @param DATE or DATETIME $end
	 * @param String $title
	 * @param HTML $description
	 * @param String $location
	 * @param Number $sequence_id - increment sequence id when event is updated
	 * @param obj $timeZone - instance of TimeZone, optional. If provided, set timezone properties
	 */
	function __construct($id, $start, $end, $title, $description = "", $location = "", $sequence_id = 0, $timeZone = null)
	{
		global $config;
				
		$this->data = new iCalendarData();
		$this->data->set("summary", $title);
		$this->data->set("description", $description);
		$this->data->set("location", $location);

		// The uid must be globally unique thus is includes the http_host
		// and the event id.
		$uid = $id . $config["http_host"];
		$this->data->set("uid", $uid);
		
		$this->data->set("sequence", $sequence_id);
		
		$this->data->set("dtstart", $start);
		$this->data->set("dtend", $end);
		
		$today = new DateTime(now());	
		$this->data->set("dtstamp", $today->format('Ymd\THis'));
		
		$this->data->filter = new ExclusionFilter("last_modified", "standard_rrule", "daylight_rrule");	
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
				
		$this->setEventDates($start, $end, $timeZone);

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
	function setEventDates($start, $end, $timeZone)
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
		
		if($timeZone)
		{
			$this->setTimeZone($timeZone, $start_value);
			$this->data->set("dtstart", ";TZID=" . $timeZone->time_zone_name . ":". $start_value);
			$this->data->set("dtend", ";TZID=" . $timeZone->time_zone_name . ":". $end_value);
		}
		else
		{
			$this->excludeTimeZoneFields();
			$this->data->set("dtstart", $start_value);
			$this->data->set("dtend", $end_value);
		}
	}
	
	/**
	 * If time zone is not provided, exlcude the time zone fields
	 * so they are not included in the output.
	 * 
	 * If provided, set the values.
	 * 
	 * @param obj $timeZone class TimeZone
	 */
	function setTimeZone($timeZone, $start_value)
	{
		$this->data->set("begin_timezone", "VTIMEZONE");
		$this->data->set("begin_standard", "STANDARD");
		$this->data->set("end_standard", "STANDARD");
		$this->data->set("begin_daylight", "DAYLIGHT");
		$this->data->set("end_daylight", "DAYLIGHT");
		$this->data->set("end_timezone", "VTIMEZONE");
		
		$this->data->set("tzid", $timeZone->get("time_zone_name"));
		
		$standard_offset = $this->formatTimeZoneHours($timeZone->standard_offset);
		$daylight_offset = $this->formatTimeZoneHours($timeZone->daylight_offset);
		
		$this->data->set("standard_name", "EST");
		$this->data->set("daylight_name", "EDT");
		$this->data->set("standard_offset", $standard_offset);
		$this->data->set("standard_dst_offset", $daylight_offset);
		$this->data->set("daylight_offset", $daylight_offset);
		$this->data->set("daylight_dst_offset", $standard_offset);	
		
		$this->data->set("standard_start", $start_value);
		$this->data->set("daylight_start", $start_value);
	}
	
	/**
	 * Given the numeric time zone value (pos or neg), convert to
	 * hour format for iCalendar such as
	 * "-0700"
	 * 
	 * Noninteger time zones offset are rare and none are segments
	 * other than the half hour. If non integer, set the min to 30
	 * regardless of the entered value.
	 * 
	 * @param Number $offset
	 * 
	 * @return String
	 */
	function formatTimeZoneHours($offset)
	{
		$neg = false;
		if($offset < 0)
		{
			$neg = true;
			$offset = abs($offset);
		}

		$hour = $offset;
		$min = "00";
		if(!is_int($offset))
		{
			$hour = floor($offset);
			$min = "30";
		}
		
		$out = str_pad($hour, 2, STR_PAD_LEFT) . $min;
		
		if($neg)
		{
			$out = "-" . $out;
		}
		
		return $out;
	}
	
	/**
	 * If no time zone provided, exclude all fields between
	 * begin_timezone through end_timezone.
	 */
	function excludeTimeZoneFields()
	{
		$filter = $this->data->getFilter();
		if(!$filter)
		{
			$filter = new ExclusionFilter();
		}
			
		$fields = $this->data->getFields();
		$tz_field = false;
		foreach($fields as $name => $type)
		{
			if($name == "begin_timezone")
			{
				$tz_field = true;
			}
			if($tz_field)
			{
				$filter->excludeField($name);
			}
			if($name == "end_timezone")
			{
				$tz_field = false;
			}
		}
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
		
	/**
	 * Called from EmailHandler to set the attendee email address. The code that
	 * calls the EmailHandler can set the attendee name if known.
	 * 
	 * @param String $mailTo
	 */
	function setAttendee($mailTo)
	{
		$this->mailTo = $mailTo;;
	}
	
	function setAttendeeName($name)
	{
		$this->attendeeName = $name;
	}
		
	/**
	 * We wait until final formatting of the iCalendar data to
	 * set the organizer properties because values can be set
	 * after the iCalendar object is instantiated.
	 * 
	 * EmailHandler calls setOrganizer to set the replyTo
	 * email address and replyToName
	 * 
	 * Example: ORGANIZER;CN=John Smith:MAILTO:jsmith@host1.com
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
	
	/**
	 * Given the icalendar data values and the var settings,
	 * format the iCalendar data into a list of properties
	 * and values in the field order of the iCalendarData object
	 * values. This format function is called from the EmailHandler
	 * when formatting the message.
	 * 
	 * Example:
	 * 
	 * BEGIN:VCALENDAR
	 * PRODID:-//Microsoft Corporation//Outlook 11.0 MIMEDIR//EN
	 * VERSION:2.0
	 * METHOD:METHOD
	 * BEGIN:VEVENT
	 * ORGANIZER;CN=Web Site Administrator:MAILTO:andy@sonjara.com
	 * ATTENDEE;RSVP=true;CN=Janice Gallant:MAILTO:janice@sonjara.com
	 * DTSTART;VALUE=DATE:20130328
	 * DTEND;VALUE=DATE:20130328
	 * LOCATION:test
	 * TRANSP:OPAQUE
	 * STATUS:CONFIRMED
	 * X-MICROSOFT-CDO-INTENDEDSTATUS:BUSY
	 * UID:243stem2stern
	 * SEQUENCE:2
	 * DTSTAMP:20130321T120233
	 * DESCRIPTION:test
	 * SUMMARY:test all day
	 * PRIORITY:5
	 * CLASS:PUBLIC
	 * END:VEVENT
	 * END:VCALENDAR
	 * 
	 * @return string
	 */
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
/**@}*/?>
