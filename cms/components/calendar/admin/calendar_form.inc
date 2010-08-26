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

$readList = new CheckListFieldRenderer($form, "read_access", "Read Access", SiteRole::getRolesArray());
$writeList = new CheckListFieldRenderer($form, "write_access", "Write Access", SiteRole::getRolesArray());

$tabs = calendarTabs($calendar_id);

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