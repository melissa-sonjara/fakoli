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

Fakoli::using("calendar", "role");
Fakoli::usingFeature("auto_form");

$menu_item = "Calendars";

$calendar_id = checkNumeric($_GET["calendar_id"]);

$eventTypes = query(EventType, "order by name");

if ($calendar_id)
{
	$calendar = new Calendar($calendar_id);
	$title = "Calendar Details";
}
else
{
	$calendar = new Calendar();
	$title = "Add a New Calendar";
	$calendar->owner_id = $user->user_id;
}

$form = new AutoForm($calendar);
$form->hide("owner_id");

$calendar_event_type_list = new CrossReferenceSelectFieldRenderer($form, "event_type", "Event Types", $eventTypes, "name", CalendarEventTypeXref);
$readList = new CheckListFieldRenderer($form, "read_access", "Read Access", SiteRole::getRolesArray());
$writeList = new CheckListFieldRenderer($form, "write_access", "Write Access", SiteRole::getRolesArray());

$tabs = CalendarManager::getTabs($calendar_id);

if ($method == "POST")
{
	if ($form->save())
	{
		$tabs->queryString = "calendar_id={$form->data->calendar_id}";
		$tabs->next();
	}
}

$script = $form->writeScript();



$tabs->writeHTML();
?>
<div style="clear:left; border: solid 1px #000; width: 100%; margin: 0; padding: 4px;">
<?
$form->drawForm();
?>
</div><br/>
<?