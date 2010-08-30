<?php
/* =======================================================================
 * For further information please contact Andy Green at andy@sonjara.com
 * =======================================================================
 */

Fakoli::using("calendar");
Fakoli::using("site");
Fakoli::usingFeature("auto_form");

Fakoli::assertRole("admin");

$calendar_id = checkNumeric($_GET["calendar_id"]);

$menu_item = "Events";

$event = new Event();

$form = new AutoForm($event);
$form->required("title", "start_date", "end_date");
$form->hide("composite_class");

$form->allowDelete = true;

$sites = query(Site, "ORDER BY site_name");

$siteSelect = new CrossReferenceSelectFieldRenderer($form, "sites", "Publish to Sites", $sites, "site_name", EventSiteXref);
$calendarSelect = new RelatedItemSelectFieldRenderer($form, "calendar_id", "Calendar", Calendar, "ORDER BY name", "{name}", "calendar_id", false, false, 80);
$eventTypeSelect = new SelectFieldRenderer($form, "event_type", "Event Type");
$eventTypeSelect->allowAddEntry();

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
