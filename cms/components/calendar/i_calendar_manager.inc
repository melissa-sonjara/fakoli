<?php

class iCalendarEventManager
{
	var $properties = array();
	
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
		$this->setValue("METHOD", "PUBLISH");
		$this->setValue("BEGIN_2", "VEVENT");
		
		$this->setOrganizer("");
		$this->setStart($start);
		$this->setEnd($end);
		$this->setLocation($location);
		$this->setRSVP("");
		
		$this->setValue("TRANSP", "OPAQUE");
		
		$this->setSequence($sequence_id);
		$this->setEventID($id);
		
		$today = new DateTime(now());
		$today = $today->format('Ymd\THis');
		
		$this->setValue("DTSTAMP", $today);
		
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
	
	function setReplyTo($replyTo, $rsvp = true)
	{
		$this->setOrganizer($replyTo);
		if($rsvp)
		{
			$this->setRSVP($replyTo);
		}
	}
	
	function setOrganizer($replyTo)
	{
		$this->setValue("ORGANIZER:MAILTO", $replyTo);
	}
	
	function setRSVP($replyTo)
	{
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
		$out .= implode("\r\n", $properties);
		$out .= "\r\n";
		
		$out = preg_replace("/BEGIN_2:/i", "BEGIN:", $out);
		$out = preg_replace("/END_2:/i", "END:", $out);
		
		return $out;
	}
}