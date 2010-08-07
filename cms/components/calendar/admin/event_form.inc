<?php
/* =======================================================================
 * For further information please contact Andy Green at andy@sonjara.com
 * =======================================================================
 */

Fakoli::using("calendar");
Fakoli::using("site");
Fakoli::usingFile("framework/auto_form.inc");

Fakoli::assertRole("admin");

$menu_item = "Events";

$event = new Event();

$form = new AutoForm($event);
$form->required("title", "start_date", "end_date");
$form->readonly("composite_class");
$form->allowDelete = true;

$sites = query(Site, "ORDER BY site_name");

$siteSelect = new CrossReferenceSelectFieldRenderer($form, "sites", "Publish to Sites", $sites, "site_name", EventSiteXref);

if ($method == "POST")
{
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
