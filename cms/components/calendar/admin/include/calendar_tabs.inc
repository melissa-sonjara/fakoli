<?php
Fakoli::usingFeature("tab_bar");

function calendarTabs($key)
{
	$tabs = array(	"Details"			=>	"/admin/calendar_form",
					"Calendar"			=>	"/admin/calendar_view",
					"Event List"		=>	"/admin/calendar_events"
					);

	$qs = ($key) ? "calendar_id=$key" : ""; 
	return new TabBar("tabs", $tabs, $qs);
}