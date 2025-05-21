<?php
/**\addtogroup calendar
 * @{
 */


/**************************************************************

 Copyright (c) 2010 Sonjara, Inc

 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:

 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.

 Except as contained in this notice, the name(s) of the above 
 copyright holders shall not be used in advertising or otherwise 
 to promote the sale, use or other dealings in this Software 
 without prior written authorization.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.

*****************************************************************/

Fakoli::using("component");
Fakoli::usingFeature("tab_bar");

class CalendarManager
{
	static function displayCalendar($identifier, &$continue)
	{
		try
		{
			$calendar = Calendar::findByIdentifier($identifier);
			
			$page = ComponentPage::findByIdentifier("calendar_view", "WHERE enabled=1");
			$_GET["cal"] = $identifier;
		
			$pageView = new ComponentPageView($page, "{$page->template}.tpl");
		
			$page_role = $page->role;
			
			if (!checkRole($page->role))
			{
				LoginManager::storeRedirectPage();
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
	
	static function enumerateItems($items)
	{
		$calendars = Query::create('Calendar', "ORDER BY name")->execute();
		
		$items["Calendars"] = $calendars;
		return $items;
	}
	
	static function enumerateContextHelpClasses($classes)
	{
		array_push($classes, "Event");
		
		return $classes;
	}
	
	static function getTabs($key)
	{
		$tabs = array(	"Details"			=>	"/admin/calendar_form",
				"Calendar"			=>	"/admin/calendar_view",
				"Event List"		=>	"/admin/calendar_events"
		);
	
		$qs = ($key) ? "calendar_id=$key" : "";
		return new TabBar("tabs", $tabs, $qs);
	}
	
	static function upgradeComponent($version)
	{
		$mgr = new CalendarUpgradeManager();
		$mgr->upgrade($version);
	}	

	/**
	 * Respond to fired event DeleteUser
	 *
	 * Delete any records in this component that have dependencies on user object.
	 *
	 * @param obj $user - class SiteUser or custom user class
	 */
	static function deleteUser($user)
	{
		$pk = $user->getPrimaryKey();
		$user_id = $user->$pk;
		
		trace("Component calendar is deleting objects dependent on user_id {$user_id}", 3);
	
		$calendar = new Calendar();
		$calendar->delete("WHERE owner_id={$user_id}");
		
		return $user;
	}
	
	static function registerTaxonomyClasses($classes)
	{
		$classes[] = 'Event';
		return $classes;
	}

	static function registerCommentAdapter()
	{
		CommentManager::registerAdapter("calendar", 'Event', 'EventCommentXref');
	}
	
	static function registerSerializationHandler()
	{
		SerializationManager::registerHandler("calendar", "Calendars, Calendar Events and Event Types", 
											  new CompoundSerializationHandler("CalendarMap", 
											  		'Calendar', 'Event', 'EventCommentXref', 
											  		'EventSiteXref', 'EventType',
											  		'CalendarEventTypeXref'));
		SerializationManager::registerHandler("timezones", "Time Zones", new SimpleSerializationHandler('TimeZone'));
		return true;
	}
	
	/**
	 * params to setDefaultValue: component, name, value, field_type, annotation, category, options
	 */
	static function setDefaults()
	{
		Settings::setDefaultValue("calendar", "max_events_per_day", 5, Number, "For calendar monthly display; set to 0 if no maximum");
		// There does not seem to be any use in a "view_events" since this would be controlled by 
		// page role and menu role access to the calendar pages.
		Settings::setDefaultPermission("calendar", "edit_events", "Add or Edit Events", "admin");
		Settings::setDefaultValue("calendar", "allow_comments", false, Boolean, "Allow users to comment on events");
	}
}

/**@}*/?>
