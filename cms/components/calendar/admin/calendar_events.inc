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
$view->addHandler(Event, new StandardEventHandler());

$view->year = $year;
$view->month = $month;

$tabs = calendarTabs($calendar_id);

$tabs->writeHTML();
?>
<div style="clear:left; border: solid 1px #000; width: 100%; margin: 0; padding: 4px;">
<?
$view->drawEventList();
?>
<br/>
<a href='/admin/event_form?calendar_id=<?echo $calendar_id?>' class='button'>Add an Event</a>
<br/>
</div>