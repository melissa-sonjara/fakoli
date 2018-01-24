<?php
/**************************************************************

 Copyright (c) 2011 Sonjara, Inc

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

class ActivityTrackerManifest
{
	static function getComponentDefinition()
	{
		$component = new Component();
		$component->name = "activity_tracker";
		$component->description = "User tracking and feedback module";
		$component->author = "Andy Green";
		$component->version = "1.0";
		$component->priority = 10;
		$component->enabled = true;
		
		return $component;
	}
	
	var $allow_sessionless_handlers = array("save");
	
	static function subscribeToEvents()
	{
		return array("RequestComplete" 			=> array(ActivityTracker, logActivity),
					 "ComponentScanComplete"	=> array(ActivityTracker, setDefaults),
					 "upgradeComponent"			=> array(ActivityTracker, upgradeComponent),
					 "PostProcessContent"		=> array(ActivityTracker, postProcessContent),
					 "DeleteUser"				=> array(ActivityTracker, deleteUser)		
				);
	}
		
	static function getAdminMenu()
	{
			return array("User Management"	=> array
			(

				"Activity Report"	=>	array("page"	=>	"/admin/activity_report",
											  "role"	=>	"admin",
									  		  "weight"	=>	10,
											  "icon"	=>	"flag-checkered"
											),
											
				"Feedback"			=>	array("page"	=>	"/admin/feedback",
										  	  "role"	=>	"admin",
									  	  	  "weight"	=>	11,
											  "icon"	=>	"far fa-comment-alt"
											)
			)
		);		
	}

	static function getScripts()
	{
		return array("/components/activity_tracker/js/feedback.js");
	}
	
	static function getStyles()
	{
		return array("/components/activity_tracker/css/feedback.css");
	}
}

?>