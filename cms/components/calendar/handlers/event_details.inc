<?php
Fakoli::using("calendar");

$event_id = checkNumeric($_GET["event_id"]);

if (!$event_id) die("Invalid Parameter");

$event = new Event($event_id);

$handler = new StandardEventHandler();

echo $handler->details($event);

if (checkRole("admin"))
{
?>
<br/>
<button class='button' onclick='editEvent(<?echo $event_id?>)'>Edit this Event</button>
<?
}
?>