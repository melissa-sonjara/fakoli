<?php
Fakoli::using("calendar");

$menu_item = "Calendars";

require_once "include/calendar_tabs.inc";

$calendar_id = checkNumeric($_GET["calendar_id"]);


if (!$calendar_id) redirect("/admin/calendars");

$calendar = new Calendar($calendar_id);

$year = checkNumeric($_REQUEST["year"]);
$month = checkNumeric($_REQUEST["month"]);

$title = "Calendar View: {$calendar->name}";

if (!$year)
{
	$year = date("Y");
}

if (!$month)
{
	$month = date("n");	
}

$startStr = sprintf("%d-%02d-01", $year, $month);
$start = strtotime($startStr);

$prevMonth = $month - 1;
$prevYear = $year;

if ($prevMonth <= 0)
{
	$prevYear--;
	$prevMonth = 12;
}

$nextMonth = $month + 1;
$nextYear = $year;
if ($nextMonth > 12)
{
	$nextMonth = 1;
	$nextYear++;
}

$endStr = sprintf("%d-%02d-01", $nextYear, $nextMonth);
$constraint = "WHERE end_date >= '$startStr' AND start_date < '$endStr' AND published=1 ORDER BY start_date";

$events = $calendar->Events($constraint);

$view = new CalendarView($events, "fullcalendar");

$view->year = $year;
$view->month = $month;

$tabs = calendarTabs($calendar_id);

$tabs->writeHTML();
?>
<div style="clear:left; border: solid 1px #000; width: 100%; margin: 0; padding: 4px;">
<?
$view->drawMonth();
?>
<br/>
<a href='/admin/event_form?calendar_id=<?echo $calendar_id?>' class='button'>Add an Event</a>
<br/>
</div>