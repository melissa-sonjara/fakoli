<?php
class CalendarHandler
{  
	var $events;
	
	function CalendarHandler()
	{
		
	}
	
	function getEvents($calendar, $constraint = "")
	{
		$constraint .= " ORDER BY start_date";
		return $calendar->Events($constraint);
	}
	
	function drawFilter() 
	{
		//nothing to do , will be overidden;
	}

	function addButtonsAtBottom($calendar)
	{
		global $user;
		
		if ($user && checkRole($calendar->write_access))
		{
		?>
		<br/>
		<a href='' onclick="newEvent(<?echo $calendar->calendar_id?>); return false;" class='button'>Add an Event</a>
			<br/>
		<?
		}
	}
}