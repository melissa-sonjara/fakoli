<?php

class CalendarHelper
{
	function CalendarHelper()
	{
		
	}
	
	static function eventFilterSetup()
	{
		$event = new Event();
		$event->fromGET();
		
		$event->filter = new InclusionFilter("calendar_id", "start_date", "end_date");
		
		$filterForm = new FilterForm($event, "GET");
		$filterForm->hide("end_date");
		
		$calendars = Query::create(Calendar, "ORDER BY name")->execute();
		if(count($calendars) > 0)
		{
			$calendarSelect = new FilterFieldRenderer($filterForm, "calendar_id", "Calendar", FilterFieldRenderer::formatOptions($calendars, "{name}"));
		}
		else
		{
			$form->hide("calendar_id");
		}
		$dateRangeRenderer = new DateRangeFilterRenderer($filterForm, "start_date", "start_date", "year");
		$dateRangeRenderer->dateRangeLabel = "Event Date";
			
		return $filterForm;
	}
}