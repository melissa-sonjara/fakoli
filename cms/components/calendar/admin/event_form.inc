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

$menu_item = "Calendar Events";

$event = new Event();

$form = new AutoForm($event);
$form->required("title", "start_date", "end_date");
$form->hide("composite_class", "event_invitation_id", "allow_access");

$form->allowDelete = true;

$sites = query(Site, "ORDER BY site_name");
if (count($sites) > 1)
{
	$siteSelect = new CrossReferenceSelectFieldRenderer($form, "sites", "Publish to Sites", $sites, "site_name", EventSiteXref);
}

$calendarSelect = new RelatedItemSelectFieldRenderer($form, "calendar_id", "Calendar", Calendar, "ORDER BY name", "{name}", "calendar_id", false, false, 80);
$eventTypeSelect = new SelectFieldRenderer($form, "event_type", "Event Type");
$eventTypeSelect->allowAddEntry();

TaxonomyManager::addTaxonomyFieldRenderers($form);

if ($method == "POST")
{
	$_POST["calendar_id"] = $calendar_id;
	if ($form->save())
	{	
		redirect("/admin/events");
	}
}
else
{
	$event_id = checkNumeric($_GET["event_id"]);
	
	if ($event_id)
	{
		$form->load($event_id);
	}
	else
	{
		$event->calendar_id = $calendar_id;
	}
}

if ($event->event_id)
{
	$title = "Edit Event";
}
else
{
	$title =  "Add New Event";
}
$script = $form->writeScript();
$section = "Calendar";



$form->drawForm();


?>
