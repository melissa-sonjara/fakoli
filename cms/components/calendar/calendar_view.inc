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

Fakoli::using("settings");

abstract class EventHandler
{
	
	function filter($event, $date)
	{
		return (strncmp($event->start_date, $date, strlen($date)) == 0);
	}
	
	abstract function formatSummary($event, $tagRowCallbacks);

	abstract function formatFilteringTags($event, $tagRowCallbacks);
	abstract function summary($event);
	abstract function details($event);
	abstract function buildForm($event);
	
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
	
	/**
	 * Called by function details
	 * 
	 * @param obj $event
	 */
	function formatButtons($event)
	{
		$event_id = $event->get("event_id");
	
		if(checkRole("admin"))
		{
			$out = "<br/><button class='button' onclick='editEvent($event_id)'>Edit this Event</button>\n";
		}	
		
		return $out;
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
	
	function formatFilteringTags($event, $tagRowCallbacks)
	{
		$pk = $event->getPrimaryKey();
		if(!count($tagRowCallbacks)) return "";
		
		$id = $event->format("id='{$pk}_{{$pk}}'");
		
		foreach($tagRowCallbacks as $cb)
		{
			$dataAttrs = call_user_func($cb, $event, $dataAttrs);
		}
		
		$attrs = "";
		foreach($dataAttrs as $name => $value)
		{
			$attrs .= " ".$name."='".$value."'";
		}
		
		return "{$id}{$attrs}";
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
<?

	echo $this->formatButtons($event);
?>
</div>
</div>
<?		
		$out = ob_get_contents();
		ob_end_clean();
		
		return $out;
	}
	
	function formatiCalendar($event, $sequence_id = 0)
	{
		return new iCalendarEventManager($event->event_id, $event->start_date, $event->end_date, $event->title, $event->description, $event->location, $sequence_id, $event->TimeZone());
	}
	
	function buildForm($event)
	{
		$form = new AutoForm($event);
		$form->required("title", "start_date", "end_date");
		$form->hide("composite_class", "event_invitation_id", "allow_access", "time_zone_id", "owner_id");
		
		$form->allowDelete = true;
		
		$sites = query(Site, "ORDER BY site_name");
		if (count($sites) > 1)
		{
			$siteSelect = new CrossReferenceSelectFieldRenderer($form, "sites", "Publish to Sites", $sites, "site_name", EventSiteXref);
		}
		
		$calendarSelect = new RelatedItemSelectFieldRenderer($form, "calendar_id", "Calendar", Calendar, "ORDER BY name", "{name}", "calendar_id", false, false, 80);
		$eventTypeSelect = new SelectFieldRenderer($form, "event_type", "Event Type");
		$eventTypeSelect->allowAddEntry();
		
		return $form;
	}
	
	function formatSummary($event, $tagRowCallacks)
	{
		$handler_class = get_class($this);
		$pk = $event->getPrimaryKey();
		$tags = $this->formatFilteringTags($event, $tagRowCallacks);
		$content .= "<div {$tags} class='event_summary' onclick='showEventDetail({$event->$pk}, \"{$handler_class}\")'>";
		$content .= $this->summary($event);
		$content .= "</div>";
	
		return $content;
	}
	
}

class CalendarView implements FacetFilterable
{
	var $current_day;
	var $events;
	var $id; 		///< The HTML tag id for the calendar view
	var	$month;
	var $year;
	var $handlerMap = array(); // key is event class, value is handler class name
	var $calendarLink = "/calendar";
	var $listLink = "/event_list";
	var $listDialog = ""; // javascript function for bubble popup
	var $detailLink = "/event_detail";
	var $yearParam = "year";
	var $monthParam = "month";
	var $maxEventsPerDay = null; ///< The maximum number of events to display per day on monthly view
	var $tagRowCallbacks; ///< Array of callbacks for adding extra attributes to each row
	
	function CalendarView($events, $id = null)
	{
		$this->events = $events;
		$this->id = $id;
		$this->month = date("n");
		$this->year = date("Y");
		$this->tagRowCallbacks = array();
		$this->addHandler(Event, new StandardEventHandler());
		ComponentManager::fireEvent("RegisterCalendarEventHandlers", $this);
		
		$max = Settings::getValue("calendar", "max_events_per_day");
		if($max)
		{
			$this->maxEventsPerDay = $max;
		}
	}

	function getID()
	{
		return $this->id;
	}
		
	/**
	 * 
	 * @param String $class composite event class or data item class name
	 * @param obj $handler instance of a class inherits from either
	 * StandardEventHandler or EventHandler
	 */
	function addHandler($class, $handler)
	{
		$this->handlerMap[$class] = $handler;
	}
	
	/*
	 * Preferred approach is for subscribes to RegisterCalendarEventHandlers
	 * to respond with function that maps a composite or event class to
	 * a handler class. Example:
	 * 	static function registerCalendarEventHandlers(&$view)
	 *  {
	 *  	$view->addHandler(CompositeRegistrationEvent, new RegistrationEventHandler());
	 * 		return $view;
	 *  }
	 *
	 */
	function getHandler($event)
	{
		if(get_class($event) == "Event" && $event->composite_class)
		{
			$event_class = $event->composite_class;
		}
		else
		{
			$event_class = get_class($event);
		}
	
		if(array_key_exists($event_class, $this->handlerMap))
		{
			$handler = $this->handlerMap[$event_class];
		}
		else
		{
			$handler = new StandardEventHandler();
		}
			
		return $handler;	
	}
	
	/**
	 * Adds a row tagging handler. This function will receive the data item
	 * for each row and can add custom attributes to the row tag.
	 */
	function addFacetTaggingHandler($handler)
	{
		$this->tagRowCallbacks[] = $handler;
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
		$this->createEventDialog();
		
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
 <td colspan="7" class="month"><a class="calendar_nav" href="<?echo $this->prevLink?>">&lt;</a>&nbsp; <?echo date("F Y", $this->start)?>&nbsp; <a class="calendar_nav" href="<?echo $this->nextLink?>">&gt;</a> </td>
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
	
	function createEventDialog()
 	{
	}
	
	function drawMonth()
	{
		$this->createEventDialog();
		
		$this->calculateMonthRange();
?>		
<div id="fullcalendar">
<table id=<?php echo $this->id ?>>
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
				foreach($eventsToday as $event)
				{
					$handler = $this->getHandler($event);
					$content .= $handler->formatSummary($event, $this->tagRowCallbacks);
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
 <td class="<?echo $class ?>" data-day="<?php echo $i ?>"><?echo $link ?><? echo $content?></td>
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
		
	function writeScript()
	{
		if(!count($this->tagRowCallbacks) || !$this->maxEventsPerDay) return;
	
		$options = "{}";
		if($this->maxEventsPerDay)
		{
			$options = "{per_day: {$this->maxEventsPerDay}}";
		}

		ob_start();
		?>
		<script type="text/javascript" src="/components/calendar/js/filtering_calendar.js"></script>
		<script type="text/javascript">
		window.addEvent('domready', function()
		{
			var <?echo $this->id?> = new FilteringCalendar('<?echo $this->id ?>', <?php echo $options ?>);
		});
		</script>
	<?
			$script = ob_get_contents();
			ob_end_clean();
			return $script;
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
		$this->createEventDialog();
		
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
/**@}*/?>