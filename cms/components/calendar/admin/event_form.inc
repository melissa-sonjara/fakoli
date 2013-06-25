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

Fakoli::using("calendar", "taxonomy");
Fakoli::using("site");
Fakoli::usingFeature("auto_form");

Fakoli::assertRole("admin");

$calendar_id = checkNumeric($_GET["calendar_id"]);
$event_id = checkNumeric($_GET["event_id"]);

$menu_item = "Calendar Events";
$section = "Calendar";

$event = new Event();

$helper = new CalendarHelper();

if ($event_id)
{
	$event->load($event_id);	
	$title = "Edit Event";
}
else
{
	$event->calendar_id = $calendar_id;
	$event->setOwner();
	$title =  "Add New Event";
}

$handler = $helper->getHandler($event);

$form = $handler->buildForm($event);

TaxonomyManager::addTaxonomyFieldRenderers($form);

if ($method == "POST")
{
	$_POST["calendar_id"] = $calendar_id;
	if ($form->save())
	{	
		redirect("/admin/events");
	}
}

$script = $form->writeScript();

$form->drawForm();
?>
