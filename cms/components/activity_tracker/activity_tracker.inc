<?php
Fakoli::using("settings");

class ActivityTracker
{
	static function logActivity()
	{
		global $user;
		if (!$user) return;

		$enabled = Settings::getValue("activity_tracker", "track_activity");
		$entryPoint = basename($_SERVER['SCRIPT_NAME']);

		if (!$enabled || ($entryPoint != 'page.php' && $entryPoint != 'action.php')) return;
		
		$activity = new UserActivity();

		$activity->user_id = $user->user_id;
		
		$activity->uri = $_SERVER['REQUEST_URI'];
		$activity->method = $_SERVER['REQUEST_METHOD'];
		$activity->referer = $_SERVER['HTTP_REFERER'];
		$activity->session_id = session_id();

		$activity->save();
	}	
	
	static function setDefaults()
	{		
		Settings::setDefaultValue("activity_tracker", "track_activity", "", Boolean, "Track user activity. Use this for usage statistics and troubleshooting. For better performance, disable this on sites with heavy traffic.");
		Settings::setDefaultValue("activity_tracker", "enable_feedback", "", Boolean, "Enable the user feedback feature. You will also need to assign the feedback module to a position in your template.");
	}
	
	static function upgradeComponent($version)
	{
		$mgr = new ActivityTrackerUpgradeManager();
		$mgr->upgrade($version);
	}
}

?>