<?php
/* =======================================================================
 * For further information please contact Andy Green at andy@sonjara.com
 * =======================================================================
 */

Fakoli::using("calendar");
Fakoli::using("site");
Fakoli::usingFeature("auto_form");

Fakoli::assertRole("admin");

$menu_item = "Events";

$event_id = checkNumeric($_GET["event_id"]);
$calendar_id = checkNumeric($_REQUEST["calendar_id"]);

if (!$event_id)
{
	$event = new Event();
	$event->calendar_id = $calendar_id;
}
else
{
	$event = new Event($event_id);
}

$form = new AutoForm($event, "POST", "/action/calendar/event_edit?event_id=$event_id");
//$event->filter = new ExclusionFilter("description");
$desc = $form->getRenderer("description");
$desc->width = "450px";
$desc->height = "120px";
$form->required("title", "start_date", "end_date");
$form->hide("composite_class");

$form->allowDelete = true;
$form->ajaxSubmit("function(result) {editEventResult(result);}", "function() {\$('{$form->id}_error').set('text','Failed to communicate with server'); }");

$sites = query(Site, "ORDER BY site_name");

$siteSelect = new CrossReferenceSelectFieldRenderer($form, "sites", "Publish to Sites", $sites, "site_name", EventSiteXref);
$calendarSelect = new RelatedItemSelectFieldRenderer($form, "calendar_id", "Calendar", Calendar, "ORDER BY name", "{name}", "{calendar_id}", false, false, 80);
$eventTypeSelect = new SelectFieldRenderer($form, "event_type", "Event Type");
$eventTypeSelect->allowAddEntry();

if ($method == "POST")
{
	if ($form->save())
	{	
		echo "1";
		return;
	}
	else
	{
		echo $form->msg;
	}
}

echo $form->writeScript();

$form->drawForm();


?>
