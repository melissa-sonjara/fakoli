<?php
Fakoli::using("calendar", "role");
Fakoli::usingFeature("auto_form");

$menu_item = "Calendars";

require_once "include/calendar_tabs.inc";

$calendar_id = checkNumeric($_GET["calendar_id"]);

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
$form->alias("role", "Allowed access");


$roleList = new CheckListFieldRenderer($form, "role", "Role", SiteRole::getRolesArray());

$tabs = calendarTabs($calendar_id);

if ($method == "POST")
{
	if ($form->save())
	{
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