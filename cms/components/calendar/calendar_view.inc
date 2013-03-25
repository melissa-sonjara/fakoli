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

Fakoli::using("settings");

abstract class EventHandler
{
	
	function filter($event, $date)
	{
		return (strncmp($event->start_date, $date, strlen($date)) == 0);
	}
	
	abstract function summary($event);
	abstract function details($event);
	
	/**
	 * format an event for sending through email as iCal event.
	 * @param obj $event
	 */
	function formatiCalendar($event)
	{
		throw new FakoliException("No iCal formatter provided");
	}
	
	function getEventClass()
	{
		return Event;
	}
	
	
	function drawEditButton($event_id)
	{
		if(checkRole("admin"))
		{
?>
<br/>
<button class='button' onclick='editEvent(<?echo $event_id?>)'>Edit this Event</button>
<?
		}	
	}	
}

class StandardEventHandler extends EventHandler
{
	/**
	 * If an event spans over more than 5 days, only
	 * show it on the first day. If a multiday event
	 * starts before the first of the month, then 
	 * show it on the first of the month only.
	 * 
	 * @see EventHandler::filter()
	 */
	function filter($event, $date)
	{
		$start = explode(" ", $event->start_date);
		$end = explode(" ", $event->end_date);
		
		$startTimeStamp = strtotime($start[0]);
		$endTimeStamp = strtotime($end[0]);
		$dateTimeStamp = strtotime($date);

		$diff = abs($endTimeStamp - $startTimeStamp);
		$days = floor($diff/(60*60*24));

		$inDate = new DateTime($date."T00:00:00");
		$startDate = new DateTime($event->start_date);
	   
		$showEvent = false;
		if($inDate->format("j") == 1 && ($startDate->format("Y-m-d") <= $inDate->format("Y-m-d")))
		{
			$showEvent = true;
		}
	    
		if($days <= 5 || $showEvent) 
		{
  			return ($dateTimeStamp >= $startTimeStamp && $dateTimeStamp <= $endTimeStamp);
		}
		else 
		{
  			return ($dateTimeStamp == $startTimeStamp);
		}
	}

	function summary($event)
	{
		return "<h4>{$event->title}</h4><p>{$event->event_type}</p>";
	}
	
	function details($event, $byProgram = false)
	{
		ob_start();
?>
<div class="event_details">
<h3><?echo $event->title?></h3>
<div class="event_contents">
<p><em><?echo $event->event_type?></em></p>
<p><label style="width: 80px">Start: </label><?echo $event->format("{start_date:short}") ?><br/>
<label style="width: 80px">End: </label><?echo $event->format("{end_date:short}") ?></p>
<? 		
		if ($event->location)
		{ 
?>
<p><label style="width: 80px">Location: </label><?echo $event->location?></p>
<?
		}
?>
<p><?echo $event->description?></p>
</div>
</div>
<?
		$out = ob_get_contents();
		ob_end_clean();
		return $out;
	}
	
	function formatiCalendar($event, $sequence_id = 0)
	{
		return new iCalendarEventManager($event->event_id, $event->start_date, $event->end_date, $event->title, $event->description, $event->location, $sequence_id);
	}
}

class CalendarView
{
	var $current_day;
	var $events;
	var $id; 		///< The HTML tag id for the calendar view
	var	$month;
	var $year;
	var $handlerMap = array(); // key is event class, value is handler class name
	var $eventHandlers = array(); // deprecated method
	var $calendarLink = "/calendar";
	var $listLink = "/event_list";
	var $listDialog = ""; // javascript function for bubble popup
	var $detailLink = "/event_detail";
	var $yearParam = "year";
	var $monthParam = "month";
	var $maxEventsPerDay = null; ///< The maximum number of events to display per day on monthly view
	
	function CalendarView($events, $id = null)
	{
		$this->events = $events;
		$this->id = $id;
		$this->month = date("n");
		$this->year = date("Y");
		$this->addHandler(Event, new StandardEventHandler());
		ComponentManager::fireEvent("RegisterCalendarEventHandlers", $this);
	}

	// deprecated - registered event handlers should send
	// string of handler class name instead
	function addHandler($class, $handler)
	{
		$this->eventHandlers[$class] = $handler;
		$handler->view = $this;
	}
	
	/*
	 * Preferred approach is for subscribes to RegisterCalendarEventHandlers
	 * to respond with function that maps a composite or event class to
	 * a handler class. Example:
	 * 	static function registerCalendarEventHandlers(&$view)
	 *  {
	 * 		$view->handlerMap["CompositeRegistrationEvent"] = "RegistrationEventHandler";
	 * 		return $view;
	 *  }
	 *
	 * For backwards compatibility, if not found, we will attempt to 
	 * retrieve a custom handler from the array "eventHandlers" with
	 * index composite class or event class.
	 *
	 */
	function getHandler($event)
	{
		// Event can be class other than "event" e.g., TrainingEvent
		if(get_class($event) == "Event" && $event->composite_class)
			$event_class = $event->composite_class;
		else // Can be class "Event" or any other obj that is calendar-based
			$event_class = get_class($event);

		trace("getting event handler for $event_class", 3);
				
		if(array_key_exists($event_class, $this->handlerMap))
		{
			$handler_class = $this->handlerMap[$event_class];
			$handler = new $handler_class($event);
		}
		// deprecated method of supplying an event handler
		elseif(array_key_exists($event_class, $this->eventHandlers))
		{
			$handler = $this->eventHandlers[$event_class];
		}
		else // event is not class or base class "Event" and no handler supplied
		{
			$pk = $event->getPrimaryKey();
			trace("CalendarView:: no event handler for event {$event->$pk} class " . get_class($event), 3);
		}
		return $handler;
	}
	
	
	function summary($event)
	{
		return $this->getHandler($event)->summary($event);
	}
	
	function details($event, $param = null)
	{
		return $this->getHandler($event)->details($event, $param);
	}
	
	function filterEvents($events, $currentDay)
	{
		$ret = array();

		foreach($events as $event)
		{
			$handler = $this->getHandler($event);
			if ($handler AND $handler->filter($event, $currentDay))
			{
				$ret[] = $event;
			}
			// If not found - function getHandler outputs trace
		}
		
		return $ret;
	}	
	
	function rewriteQueryString($year, $month)
	{
		global $isAdmin;
		
		$qs = $_SERVER['QUERY_STRING'];
		
		$identifier = $_REQUEST["identifier"];
		
		if ($isAdmin) $identifier = "/admin/$identifier";
		
		$qs = preg_replace("/identifier=[\\w_]+&*/", "", $qs);
		$qs = preg_replace("/{$this->yearParam}=\\d+&*/", "", $qs);
		$qs = preg_replace("/{$this->monthParam}=\\d+&*/", "", $qs);
			
		return "$identifier".appendToQueryString($qs, "{$this->yearParam}=$year&{$this->monthParam}=$month");
	}
	
	function calculateMonthRange()
	{	
		if ($this->year < 2005 || $this->year > 2038 || $this->month < 1 || $this->month > 12)
		{
			die("Date out of range");
		}
		
		$this->startStr = sprintf("%d-%02d-01", $this->year, $this->month);
		$this->start = strtotime($this->startStr);
		
		$this->dayOfWeek = date("w", $this->start);
		$this->daysInMonth = getMonthDays($this->month, $this->year);
		
		$this->prevMonth = $this->month - 1;
		$this->prevYear = $this->year;
		
		if ($this->prevMonth <= 0)
		{
			$this->prevYear--;
			$this->prevMonth = 12;
		}
		
		$this->nextMonth = $this->month + 1;
		$this->nextYear = $this->year;
		if ($this->nextMonth > 12)

		{
			$this->nextMonth = 1;
			$this->nextYear++;
		}

		$this->endStr = sprintf("%d-%02d-01", $this->nextYear, $this->nextMonth);

		$this->prevLink = $this->rewriteQueryString($this->prevYear, $this->prevMonth);
		$this->nextLink = $this->rewriteQueryString($this->nextYear, $this->nextMonth);	
	}
	
	function drawMiniCalendar()
	{
		global $script;
		$script .= <<<ENDSCRIPT
<script type="text/javascript" src="/components/calendar/js/calendar.js"></script>
ENDSCRIPT;
		
		$this->calculateMonthRange();
		
		$bubbles = "";	

?>
<div id="minicalendar">
<table>
 <tr>
 <td colspan="7" class="month"><a class="calendar_nav" href="<?echo $this->prevLink?>">&lt;</a>&nbsp; <?echo date("F Y", $this->start)?><a class="calendar_nav" href="<?echo $this->nextLink?>">&gt;</a> </td>
  </tr>
 <tr>
  <th width="14%">Su</th>
  <th width="14%">Mo</th>
  <th width="14%">Tu</th>
  <th width="14%">We</th>
  <th width="14%">Th</th>
  <th width="14%">Fr</th>
  <th width="14%">Sa</th>
 </tr>
 <tr>
<?
		for($c = 0; $c < $this->dayOfWeek; ++$c)
		{
?><td class="empty">&nbsp;</td><?
		}
		
		$today = date("Y-m-d");
		
		for($i = 1; $i <= $this->daysInMonth; ++$i)
		{
			
			$currentDay = sprintf("%d-%02d-%02d", $this->year, $this->month, $i);
			$eventsToday = $this->filterEvents($this->events, $currentDay);
			
			if (count($eventsToday) > 0)
			{
				$class = "event";
				$event = $eventsToday[0];
				$eventBubbleID = "event_bubble_".str_replace("-", "_", $currentDay);
				$dayTitle = date("F jS Y", strtotime($currentDay));
				
				$link = $this->formatEventListLink($this->listLink, $this->listDialog, $currentDay, $eventBubbleID, $i);
				
				$bubbles .= 
				"<div id='$eventBubbleID' class='event_bubble'>
					<div class='event_bubble_content'>
					<h4>{$dayTitle}</h4>";
				
				foreach($eventsToday as $event)
				{
					$handler = $this->getHandler($event);
					$bubbles .= $handler->summary($event);
				}
					
				$bubbles .=
				"	</div>
				 <div class='event_bubble_bottom'>&nbsp;</div></div>";
			}
			else
			{
				$class = (($c % 7) == 0 || ($c % 7) == 6) ? "weekend" : "weekday";
				$link = $i;
			}
			
			if ($currentDay == $today) $class="today";
?>
 <td class="<?echo $class ?>" ><?echo $link ?></td>
<?
			 ++$c;
			 if (($c % 7) == 0) echo "</tr><tr>";
		}
		
		while ($c % 7)
		{

?><td class="empty">&nbsp;</td>
<?
			$c++;
		}
?>
</tr>
</table>
</div>
<?echo $bubbles?>
<?	
	}
	
	
	function drawMonth()
	{
		global $script;
		$script .= <<<ENDSCRIPT
<script type="text/javascript" src="/components/calendar/js/calendar.js"></script>
ENDSCRIPT;

		global $dialogs;
		ob_start();
?>
<div class="dialog" id="eventDetailDlg" style="width: 500px">
 <div class="dialog_header" id="eventDetailDlgHeader">
  <div style="padding: 4px;">
   <div style="float: right">&nbsp;<a id='closeEventDetailDlg' href="javascript:eventDialog.hide();">Close &times;</a></div>
   <span style="font-weight: bold" id="eventDetailDlgTitle">Event Detail</span>
  </div>	
 </div>
 <div id="eventDetailDlgBody" class="dialog_body">
 </div>
</div>

<?
		$dialogs .= ob_get_contents();
		ob_end_clean();
		
		$this->calculateMonthRange();
?>		
<div id="fullcalendar">
<table>
 <tr>
  <td class="month" colspan="7"><?echo date("F Y", $this->start)?> <a class="calendar_nav" href="<?echo $this->prevLink?>"><img src="/components/calendar/images/calendar_arrow_l.png" alt="left" width="22" height="22" border="0"></a> <a class="calendar_nav" href="<?echo $this->nextLink?>"><img src="/components/calendar/images/calendar_arrow_r.png" alt="right" width="22" height="22" border="0"></a> </td>
 </tr>
 <tr>
  <th width="14.28%">Su</th>
  <th width="14.28%">Mo</th>
  <th width="14.28%">Tu</th>
  <th width="14.28%">We</th>
  <th width="14.28%">Th</th>
  <th width="14.28%">Fr</th>
  <th width="14.28%">Sa</th>
 </tr>
 <tr>
<?
		for($c = 0; $c < $this->dayOfWeek; ++$c)
		{
?><td class="empty">&nbsp;</td><?
		}
		
		$today = date("Y-m-d");
		
		for($i = 1; $i <= $this->daysInMonth; ++$i)
		{
			
			$currentDay = sprintf("%d-%02d-%02d", $this->year, $this->month, $i);
			$eventsToday = $this->filterEvents($this->events, $currentDay);
			
			$content = "";
			
			if (count($eventsToday) > 0)
			{
				$class = "event";
				$idx = 0;
				$showMoreLink = false;
	
				foreach($eventsToday as $event)
				{
					if($this->maxEventsPerDay && $idx >= $this->maxEventsPerDay)
					{
						$showMoreLink = true;
						break;
					}
					$handler = $this->getHandler($event);
					
					// JDG 2/24/2011 send handler class to event details
					$handler_class = get_class($handler);
					$pk = $event->getPrimaryKey();
					$content .= "<div class='event_summary' onclick='showEventDetail({$event->$pk}, \"{$handler_class}\")'>";
					$content .= $handler->summary($event);
					$content .= "</div>";
					
					$idx++;
				}
				
				if($showMoreLink)
				{
					$event_ids = formatItems(reindexList($eventsToday, "{$pk}"), "{{$pk}}", ":");
					$content .= "<div class='events_more'><a href='#' onclick=\"showEventList('$event_ids', '$handler_class'); return false;\">more...</a></div>\n";
				}
				
				$link = $i;
			}
			else
			{
				$class = (($c % 7) == 0 || ($c % 7) == 6) ? "weekend" : "weekday";
				$link = $i;
			}
			
			if ($currentDay == $today) $class="today";
?>
 <td class="<?echo $class ?>" ><?echo $link ?><? echo $content?></td>
<?
			 ++$c;
			 if (($c % 7) == 0) echo "</tr><tr>";
		}
		
		while ($c % 7)
		{

?><td class="empty">&nbsp;</td>
<?
			$c++;
		}
?>
</tr>
</table>
</div>		
<?
	}
		
	function header($alt = event_display)
	{
		$link = $this->rewriteQueryString($this->year, $this->month, $alt);
		$altTitle = ($alt == event_display) ? "List of Events" : "Calendar";
?>		
<table style='width: 100%' class="layout">
 <tr>
  <td><p><a href="<? echo $link ?>">View <? echo $altTitle ?></a></p></td>
  <td align="right">
   <form method="get" action="">
    <?php
    $calendar_id = htmlSafe(checkNumeric($_GET["calendar_id"]));
    if($calendar_id)
    {
   		echo "<input id='calendar_id' type='hidden' name='calendar_id' value='$calendar_id'/>\n";
    }
   	?>
    <select name="month">
<?
	for($i = 1; $i <= 12; ++$i)
	{
		$monthName = date("F", strtotime("$i/1/2007"));
		option($i, $monthName, $i == $this->month);
	}
?>
    </select>&nbsp;<select name="year">
<?
	$endYear = date("Y") + 2;
	for($i = 2006; $i <= $endYear; ++$i)
	{
		option($i, $i, (int)$this->year);
	}
?>
    </select>
    <input type="submit" class="button" value=" GO "/>
   </form>
  </td>
 </tr>
</table>
<?
		
	}
	
	/*
	 * Link to the day's events from mini calendar
	 */
	function formatEventListLink($link, $dLink, $currentDay, $eventBubbleID, $idx)
	{
		if($link)
		{
			$link .= (preg_match("/\?/", $link)) ? "&" : "?";
			$link .= "date=$currentDay";
		}
		else
			$link = "#";
		
		if($dLink)
		{
			if(preg_match("/[\(.*?\)]$/", $dLink))
				$dLink = preg_replace("/[\(.*?\)]$/", ", '$currentDay')", $dLink);
			else
				$dLink .= "('$currentDay')";
			$onclick = "onclick=\"$dLink; return false;\"";
		}
		
		return "<a class='eventlink' href=\"{$link}\" $onclick; 
		onmouseover='showEventBubble(\"$eventBubbleID\", this);' onmouseout='hideEventBubble(\"$eventBubbleID\");'>{$idx}</a>";
	}
	
	
	function drawEventList()
	{
		$this->calculateMonthRange();
		
		$this->header(calendar_display);

		if (count($this->events) > 0)
		{
			foreach($this->events as $event)
			{
				$handler = $this->getHandler($event);
				$content .= $handler->details($event);
				$content .= "<br>";
			}
			echo $content;
		}
		else
		{
?>
<p><i>No events have been scheduled for this month.</i></p>
<?
		}
	}
	
}
?>
