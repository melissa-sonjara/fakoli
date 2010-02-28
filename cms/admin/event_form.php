<?php
/* =======================================================================
 * For further information please contact Andy Green at andy@sonjara.com
 * =======================================================================
 */

$page_role = "admin";

require_once "../../include/config.inc";
require_once "../../include/permissions.inc";
require_once "../datamodel/event.inc";
require_once "../../framework/auto_form.inc";

$event = new Event();

$form = new AutoForm($event);
$form->required("title", "start_date", "end_date");
$form->allowDelete = true;

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
