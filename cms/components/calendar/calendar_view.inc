<?php

abstract class EventHandler
{
	
	function filter($event, $date)
	{
		return (strncmp($event->start_date, $date, strlen($date)) == 0);
	}
	
	abstract function summary($event);
	abstract function details($event);
}

class CalendarView
{
	var $current_day;
	var $events;
	var $id;
	var	$month;
	var $year;
	var $eventHandlers = array();
	var $calendarLink = "/calendar";
	var $listLink = "/event_list";
	var $detailLink = "/event_detail";
	var $yearParam = "year";
	var $monthParam = "month";
	
	function CalendarView($events, $id = null)
	{
		$this->events = $events;
		$this->id = $id;
		$this->month = date("n");
		$this->year = date("Y");
	}

	function addHandler($class, $handler)
	{
		$this->eventHandlers[$class] = $handler;
		$handler->view = $this;
	}
	
	function getHandler($event)
	{
		$cl = $event->composite_class;
		if (!$cl) $cl = get_class($event);
		trace("getting event handler for $cl", 3);
		return $this->eventHandlers[$cl];
	}
	
	function formatTime12($time)
	{
		$time = explode(" ", $time);
		$time =  strtotime($time[1]);
		return date("g:ia", $time);
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
			
			if ($handler->filter($event, $currentDay))
			{
				$ret[] = $event;
			}
		}
		
		return $ret;
	}	
	
	function rewriteQueryString($year, $month)
	{
		$qs = $_SERVER['QUERY_STRING'];
		
		$identifier = $_REQUEST["identifier"];
		
		$qs = preg_replace("/identifier=[\\w_]+&*/", "", $qs);
		$qs = preg_replace("/{$this->yearParam}=\\d+&*/", "", $qs);
		$qs = preg_replace("/{$this->monthParam}=\\d+&*/", "", $qs);

		return "/$identifier".appendToQueryString($qs, "{$this->yearParam}=$year&{$this->monthParam}=$month");
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
<script type="text/javascript" src="/cms/components/calendar/calendar.js"></script>
ENDSCRIPT;
		
		$this->calculateMonthRange();
		
		$bubbles = "";	

?>
<div id="minicalendar">
<table>
 <tr>
  <td class="month"><a class="calendar_nav" href="<?echo $this->prevLink?>">&lt;</a></td>
  <td class="month" colspan="5"><?echo date("F Y", $this->start)?></td>
  <td class="month"><a class="calendar_nav" href="<?echo $this->nextLink?>">&gt;</a></td>
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
			
			if (count($eventsToday) > 0)
			{
				$class = "event";
				$event = $eventsToday[0];
				$eventBubbleID = "event_bubble_".str_replace("-", "_", $currentDay);
				$dayTitle = date("F jS Y", strtotime($currentDay));
				
				$link = "<a class='eventlink' href='{$this->listLink}?date=$currentDay' onmouseover='showEventBubble(\"$eventBubbleID\", this);' onmouseout='hideEventBubble(\"$eventBubbleID\");'>{$i}</a>";
				
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
		$this->calculateMonthRange();
?>		
<div id="fullcalendar">
<table>
 <tr>
  <td class="month"><a class="calendar_nav" href="<?echo $this->prevLink?>">&lt;</a></td>
  <td class="month" colspan="5"><?echo date("F Y", $this->start)?></td>
  <td class="month"><a class="calendar_nav" href="<?echo $this->nextLink?>">&gt;</a></td>
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
					
					$content .= "<div class='event_summary'>";
					$content .= $handler->summary($event);
					$content .= "</div>";
				}					
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
}
?>
