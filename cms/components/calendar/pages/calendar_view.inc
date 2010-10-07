<?php
Fakoli::using("calendar");

$identifier = $_GET["cal"];

$calendar = Calendar::findByIdentifier($identifier);

if (!checkRole($calendar->read_access)) redirect("/");

$year = checkNumeric($_REQUEST["year"]);
$month = checkNumeric($_REQUEST["month"]);

$page->page_title = "{$calendar->name}";

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

$view->drawMonth();

if ($user && checkRole($calendar->write_access))
{
?>
<br/>
<a href='/admin/event_form?calendar_id=<?echo $calendar->calendar_id?>' onclick="newEvent(<?echo $calendar->calendar_id?>); return false;" class='button'>Add an Event</a>
<br/>
<?
}
?>