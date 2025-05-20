<?php
/**\addtogroup calendar
 * @{
 */



class CalendarHelper
{
	var $handlerMap = array();
	
	function __construct()
	{
		ComponentManager::fireEvent("RegisterCalendarEventHandlers", $this);
	}
	
	/**
	 * If the event DataItem class is Event and the instance has a composite
	 * class, then find the handler that renders that composite class.
	 * e.g., CompositeProgramEvent could be rendered by ProgramEventhandler
	 * 
	 * If the event class is not Event, then the map should link the data item
	 * class to the handler 
	 * e.g., MyEventClass rendered by MyEventHandler
	 * 
	 * @param obj $event class Event or any obj that can be rendered on the calendar
	 * 
	 * @return obj handler instance of StandardEventHandler or class inheriting 
	 * 	from it or class EventHandler
	 */
	function getHandler($event)
	{
		$pk = $event->getPrimaryKey();
		if(!$event->$pk && count($this->handlerMap))
		{
			$handler_class = array_shift(array_values($this->handlerMap));
			return new $handler_class;
		}
		
		if(get_class($event) == "Event" && $event->composite_class)
		{
			$event_class = $event->composite_class;
		}
		else
		{
			$event_class = get_class($event);
		}
	
		if(array_key_exists($event_class, $this->handlerMap))
		{
			$handler = $this->handlerMap[$event_class];
		}
		else
		{
			$handler = new StandardEventHandler();
		}
		
		return $handler;
	}
	
	/**
	 *
	 * @param String $class composite event class or data item class name
	 * @param obj $handler instance of a class inherits from either
	 * StandardEventHandler or EventHandler
	 */
	function addHandler($class, $handler)
	{
		$this->handlerMap[$class] = $handler;
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
			$filterForm->hide("calendar_id");
		}
		$dateRangeRenderer = new DateRangeFilterRenderer($filterForm, "start_date", "start_date", "year");
		$dateRangeRenderer->dateRangeLabel = "Event Date";
			
		return $filterForm;
	}
	
	static function createCalendarFacet()
	{
		$facet = new MultiSelectFacetFilter("calendar_id", array(Calendar	=>	"{name}"));
	
		$facet->dropdownMaxWidth = "160px";
		$facet->width = "200px";
		$facet->height = "240px";
	
		return $facet;
	}	
}


/**@}*/?>
