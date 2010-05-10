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

require_once "../../include/config.inc";
require_once "../../include/permissions.inc";
require_once "../datamodel/event.inc";

$title = "Events";

$events = query(Event, "ORDER BY start_date");
$lastUpdated = "";

require_once admin_begin_page;

if (count($events) > 0)
{
?>
<table class="list" width="100%">
 <tr>
  <th>Event</th>
  <th>Location</th>
  <th>Start Date</th>
  <th>End Date</th>
 </tr>
<?
	foreach($events as $event)
	{

?>
 <tr>
  <td><b><a title="<?echo $event->title ?>" href="event_form.php?event_id=<?echo $event->event_id ?>"><?echo ellipsis($event->title, 80) ?></a></b></td>
  <td><? echo $event->location ?></td>
  <td><? echo $event->reformatFromSQLDate($event->start_date) ?></td>
  <td><? echo $event->reformatFromSQLDate($event->end_date) ?></td>
 </tr>
<?
	}
?></table>
<?
}
else
{
	echo "<i>No Events defined</i><br>";
}
?>
<br>
<input type="button" class="button" value="  Add a new Event  " onclick="go('event_form.php');">
<?
require_once admin_end_page;
?>