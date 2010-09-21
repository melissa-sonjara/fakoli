<?php

Fakoli::using("component");

class CalendarManager
{
	static function displayCalendar($identifier, &$continue)
	{
		try
		{
			$page = ComponentPage::findByIdentifier("calendar_view", "WHERE enabled=1");
			$_GET["cal"] = $identifier;
		
			$pageView = new ComponentPageView($page, "{$page->template}.tpl");
		
			$page_role = $page->role;
			
			if (!checkRole($page->role))
			{
				redirect("/login");
			}
			
			echo $pageView->drawView();
			
			$continue = false;
		}
		catch(DataNotFoundException $e)
		{
			
		}
		
		return $identifier;
	}
	
	static function enumerateItems(&$items)
	{
		$calendars = Query::create(Calendar, "ORDER BY title")->execute();
		
		$items["Calendars"] = $calendars;
		return $items;
	}
}

?>