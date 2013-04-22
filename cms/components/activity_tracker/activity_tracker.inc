<?php
Fakoli::using("settings");

class ActivityTracker
{
	static function logActivity()
	{
		global $user;
		//if (!$user) return;

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
		Settings::setDefaultValue("activity_tracker", "google_analytics_tracking_id", "", String, "Leave blank if tracking is not desired.");
	}
	
	static function upgradeComponent($version)
	{
		$mgr = new ActivityTrackerUpgradeManager();
		$mgr->upgrade($version);
	}
	
	/**
	 * Retrieve the script to setup google analytics tracking and
	 * store it in settings table.
	 */
	static function getGoogleAnalyticsScript($id)
	{
		$script = "";
			
		ob_start();
?>		
 <script type="text/javascript">

  var _gaq = _gaq || [];
  _gaq.push(['_setAccount', '<?php echo $id ?>']);
  _gaq.push(['_trackPageview']);

  (function() 
	{
    	var ga = document.createElement('script'); ga.type = 'text/javascript'; ga.async = true;
    	ga.src = ('https:' == document.location.protocol ? 'https://ssl' : 'http://www') + '.google-analytics.com/ga.js';
   		var s = document.getElementsByTagName('script')[0]; s.parentNode.insertBefore(ga, s);
  	})();

</script> 
<?php		
		$script .= ob_get_contents();
		ob_end_clean();
		
		return $script;
	}
	
	/**
	 * Called by ComponentPageView and PageView. Will append the
	 * google analytics script to the end of the page's script variable.
	 * 
	 * Content is passed by not modified here.
	 * 
	 * @param text $content
	 */
	static function postProcessContent($content)
	{
		global $script;
			
		$id = Settings::getValue("activity_tracker", "google_analytics_tracking_id");
		
		if($id)
		{
			$script .= ActivityTracker::getGoogleAnalyticsScript($id);
		}
			
		return $content;
	}	
}

class ActivityReportFilterHelper
{
	function __construct()
	{
		
	}
	
	function setup()
	{
		$session = new ActivitySession();
		$session->filter = new InclusionFilter("session_start", "session_end", "feedback_user");
		$session->fields["feedback_user"] = Boolean;
		$session->fromGET();
		
		$filterForm = new FilterForm($session);
		$filterForm->hide("startDate", "session_end");
		$dateRangeFilter = new DateRangeFilterRenderer($filterForm, "session_start", "session_end", "month");
		$userCheckBox = new BooleanFilterFieldRenderer($filterForm, "feedback_user", "Show only users that provided feedback", 1);
		$filterForm->setHandler("feedback_user", array($this, getFeedbackUserClause));
		
		return $filterForm;
	}
	
	/**
	 * For activity report filter
	 *
	 * @param String $name
	 * @param Boolean $value
	 * @return string
	 */
	static function getFeedbackUserClause($name, $value)
	{
		if(!$value) return;
		return "session_id IN (SELECT DISTINCT session_id from user_activity WHERE uri like '/action/feedback/save%')";
	}	
}

?>