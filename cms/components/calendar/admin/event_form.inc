<?php
/* =======================================================================
 * For further information please contact Andy Green at andy@sonjara.com
 * =======================================================================
 */

Fakoli::using("calendar");
Fakoli::using("site");
Fakoli::usingFile("framework/auto_form.inc");

Fakoli::assertRole("admin");

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
		redirect("events.php");
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

require_once admin_begin_page;

$form->drawForm();

require_once admin_end_page;
?>
