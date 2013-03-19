<?php

class iCalendarEventManager
{
	var $properties = array();
	var $replyTo;
	var $replyToName;
	var $attendeeName; 				///< optionally include display name of recipient
	var $rsvp = true;				///< organizer wants reply
	var $attendeeStatus;			///< PARSTAT=TENTATIVE, PARSTAT=NEEDS-ACTION
	var $required = false;			///< ROLE=REQ-PARTICIPANT if required to attend
	
	var $eventStatus = "CONFIRMED"; //< other statuses: TENTATIVE, CANCELLED
	var $invitees = array(); 		///< optionally show others invited to event
	
	
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
		$this->setValue("BEGIN", "VCALENDAR");
		$this->setValue("PRODID", "-//Microsoft Corporation//Outlook 11.0 MIMEDIR//EN");
		$this->setValue("VERSION", "2.0");
		$this->setValue("METHOD", "REQUEST");
		$this->setValue("BEGIN_2", "VEVENT");
		
		$this->setOrganizer("");
		$this->setStart($start);
		$this->setEnd($end);
		$this->setLocation($location);
		
		$this->setValue("TRANSP", "OPAQUE");
		$this->setValue("STATUS", $this->eventStatus);
		$this->setValue("X-MICROSOFT-CDO-INTENDEDSTATUS", "BUSY");
		
		$this->setSequence($sequence_id);
		
		$today = new DateTime(now());
		$today = $today->format('Ymd\THis');

		list($username, $domain) = split('@', Settings::getValue("email", "email_from"));
		$uid = $id . "@".$domain;
		$this->setEventID($uid);
		
		$this->setValue("DTSTAMP", $today);
		// needed?
		$this->setValue("LAST-MODIFIED", $today);
		
		$this->setDescription($description);
		$this->setTitle($title);

		$this->setValue("PRIORITY", "5");
		$this->setValue("CLASS", "PUBLIC");
	}
	
	function setStart($start)
	{
		$dt_start = "";
		if($start)
		{
			$start = new DateTime($start);
			$dt_start = $start->format('Ymd\THis');		
		}

		$this->setValue("DTSTART", $dt_start);
	}
	
	function setEnd($end)
	{
		$dt_end = "";
		
		if($end)
		{
			$end = new DateTime($end);
			$dt_end = $end->format('Ymd\THis');
		}
		
		$this->setValue("DTEND", $dt_end);
	}
	
	function setTitle($title)
	{
		$this->setValue("SUMMARY", $title);
	}

	function setAttachment($path)
	{
		//ATTACH;FMTTYPE=image/jpeg:http://domain.com/images/bastille.jpg
	}
	
	function setOrganizer($replyTo)
	{
		$this->setValue("ORGANIZER:MAILTO", $replyTo);
		$this->setValue("ATTENDEE;RSVP=TRUE:mailto", $replyTo);
	}
	
	function setLocation($location)
	{
		$this->setValue("LOCATION", $location);
	}
	
	function setSequence($sequence_id)
	{
		$this->setValue("SEQUENCE", $sequence_id);
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
 
	 * @param unknown $description
	 */
	function setDescription($description)
	{
		$this->setValue("DESCRIPTION", $description);
	}
	
	function setEventID($id)
	{
		$this->setValue("UID", $id);
	}
	
	function setValue($key, $value)
	{
		$this->properties[$key] = $value;
	}
	
	/**
	 * Format the VEVENT section:
	 * UID
	 * SUMMARY
	 * ORGANIZER
	 * ATTENDEE
	 * DTSTART
	 * DTEND
	 * STATUS
	 * X-MICROSOFT-CDO-INTENDEDSTATUS
	 * TRANSP
	 * LAST-MODIFIED
	 * DTSTAMP
	 * SEQUENCE
	 * DESCRIPTION
	 */
	function formatEvent()
	{
	}
	
	/**
	 * Given all the settings applied to vars, build the
	 * ATTENDEE property to be used in the calendar
	 * invitation.
	 */
	function formatAttendee()
	{
		
	}
	
	function getValue($key)
	{
		if(array_key_exists($key, $this->properties))
		{
			return $this->properties[$key];
		}
		else
		{
			return "";
		}
	}
	
	function format()
	{	
		$this->properties["END"] = "VEVENT";
		$this->properties["END_2"]  = "VCALENDAR";
		
		$properties = array();
		foreach($this->properties as $label => $value)
		{
			$properties[] = $label . ":" . $value;
		}
		
		$out = "\r\n";
		$out .= implode("\n", $properties);
		$out .= "\r\n";
		
		$out = preg_replace("/BEGIN_2:/i", "BEGIN:", $out);
		$out = preg_replace("/END_2:/i", "END:", $out);
		
		return $out;
	}
}