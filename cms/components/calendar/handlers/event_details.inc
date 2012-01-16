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

Fakoli::using("calendar", "component");

// Event id could be id to a different table than "Event"
$event_id = checkNumeric($_GET["event_id"]);
$handler_class = mysql_escape_string($_GET["handler_class"]);

if (!$event_id) die("Invalid Parameter");

trace("EventDetails handler class $handler_class", 3);
if(!$handler_class)
	$handler = new StandardEventHandler();
else
	$handler = ComponentManager::fireEvent("CalendarGetEventHandler", $handler_class);
	
if(!is_object($handler))
	$handler = new StandardEventHandler();

// Usually "Event" but could be other class
$event_class = $handler->getEventClass();
$event = new $event_class($event_id);
	
echo $handler->details($event);

$handler->drawEditButton($event_id);
?>