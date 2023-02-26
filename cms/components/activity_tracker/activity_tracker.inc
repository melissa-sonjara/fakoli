<?php
/**\addtogroup activity_tracker
 * @{
 */


Fakoli::using("settings");

class ActivityTracker
{
	static function registerActivityTracker()
	{
		trace("Registering Redirect Log Callback", 1);
		registerRedirectLogCallback(array(ActivityTracker, logActivity));
	}
	
	static function logActivity()
	{
		global $user;
		//if (!$user) return;

		$enabled = Settings::getValue("activity_tracker", "track_activity");
		$entryPoint = basename($_SERVER['SCRIPT_NAME']);

		if (!$enabled || ($entryPoint != 'page.php' && $entryPoint != 'action.php')) return;
		
		$activity = new UserActivity();

		$activity->user_id = is_object($user) ? $user->get($user->primary_key) : 0;
		
		$response_time = number_format(microtime(true) - Fakoli::$timestamp, 3);
		trace("Request completed in {$response_time}s", 3);
		$activity->uri = Fakoli::$requestURI;
		$activity->method = $_SERVER['REQUEST_METHOD'];
		$activity->referer = $_SERVER['HTTP_REFERER'];
		$activity->session_id = session_id();
		$activity->activity_time = Fakoli::$requestStartTime;
		$activity->response_time = $response_time;
		$activity->memory_usage = memory_get_peak_usage();
		
		$start = $_SESSION["ActivityTracker::session_start"];
		if (!$start)
		{
			$start = date("Y-m-d");
			$_SESSION["ActivityTracker::session_start"] = $start;
		}
		
		$log = ActivityTracker::getLogFile(session_id(), $start);
		
		$sessionLogID = $_SESSION["ActivityTracker::tracking"];
		
		if ($sessionLogID)
		{
			// Update the activity session record stats atomically
			$sessionLog = new UserActivitySession();
			$sessionLog->session_id = $sessionLogID;
			
			$update = "SET user_id=".$activity->user_id.", session_end = ".$activity->quoteFieldValue('activity_time', DateTime).", request_count = request_count + 1";
			if (startsWith($activity->uri, "/action/activity_tracker/save"))
			{
				$update .= ", feedback_count = feedback_count + 1";
			}
			
			if (startsWith($activity->uri, "/action/"))
			{
				$update .= ", action_count = action_count + 1";
			}
			else
			{
				$update .= ", page_views = page_views + 1";
			}
						
			$sessionLog->updateExplicit($update);
		}
		else
		{
			// Create the activity session record
			$sessionLog = new UserActivitySession();
			if (!$sessionLog->tableExists()) return;
			
			$sessionLog->session_identifier = session_id();
			$sessionLog->user_id = $activity->user_id;
			$sessionLog->session_start = $activity->activity_time;
			$sessionLog->session_end = $activity->activity_time;
			$sessionLog->request_count = 1;
			$sessionLog->action_count = (startsWith($activity->uri, "/action/")) ? 1 : 0;
			$sessionLog->page_views = (startsWith($activity->uri, "/action/")) ? 0 : 1;				
			$sessionLog->ip_address = $_SERVER["REMOTE_ADDR"];
			$sessionLog->user_agent = $_SERVER["HTTP_USER_AGENT"];
			$sessionLog->save();
			
			$_SESSION["ActivityTracker::tracking"] = $sessionLog->session_id;
		}
		
		error_log($activity->toJSON()."\n", 3, $log);
	}	
	
	static function getLogFile($session_identifier, $start)
	{
		$dir = Settings::getValue("activity_tracker", "logging_directory").DIRECTORY_SEPARATOR.$start;
		if (!file_exists($dir)) mkdir($dir);
		
		return $dir.DIRECTORY_SEPARATOR.$session_identifier.".json";
	}
	
	static function formatMemoryUsage($activity)
	{
		return getScaledSize($activity->memory_usage, 2);
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
		trace("Component activity_tracker is deleting objects dependent on user_id {$user_id}", 3);
		
		$tx = new DataTransaction();

		try
		{
			$feedback = new UserFeedback();
			$feedback->joinTransaction($tx);
			$feedback->delete("WHERE user_id={$user_id}");
					
			$userActivity = new UserActivity();
			$userActivity->joinTransaction($tx);
			$userActivity->delete("WHERE user_id={$user_id}");
			
			$tx->commit();
		}
		catch(Exception $e)
		{
			$tx->rollback();
			throw $e;
		}
		return $user;
	}
	
	static function setDefaults()
	{	
		global $config;
		
		$base = $config['uploadbase'].DIRECTORY_SEPARATOR."user_activity_logs";
		
		Settings::setDefaultValue("activity_tracker", "track_activity", "", Boolean, "Track user activity. Use this for usage statistics and troubleshooting. For better performance, disable this on sites with heavy traffic.", "Activity Tracking");
		Settings::setDefaultValue("activity_tracker", "enable_feedback", "", Boolean, "Enable the user feedback feature. You will also need to assign the feedback module to a position in your template.", "User Feedback", null, 1);
		Settings::setDefaultValue("activity_tracker", "feedback_title", "Provide Feedback", String, "Title text for the user feedback module", "User Feedback", null, 3);
		Settings::setDefaultValue("activity_tracker", "require_login", false, Boolean, "Whether the feedback feature is available to guest users (those who haven't yet logged in)", "User Feedback", null, 2);
		Settings::setDefaultValue("activity_tracker", "confirmation_message", "Thank you, your feedback is appreciated!", String, "The message to display after the user submits feedback", "User Feedback", null, 4);
		
		Settings::setDefaultValue("activity_tracker", "send_email", false, Boolean, "Send a notification email when feedback is submitted. If you set this to true, configure recipient and message via the 'feedback_notification' email template", "User Feedback", null, 5);
		  
		Settings::setDefaultValue("activity_tracker", "google_analytics_tracking_id", "", String, "Leave blank if tracking is not desired.", "Google Analytics");
		Settings::setDefaultValue("activity_tracker", "google_analytics_version", "Classic", String, "Specifies the API Version of Google Analytics to use. Select this based on your site's analytics configuration on google.com", "Google Analytics", "Classic\nUniversal\nGA4");
		
		Settings::setDefaultValue("activity_tracker", "logging_directory", $base, String, "Specifies the directory to which user activity logs will be written", "Activity Tracking");		
		
		ActivityTracker::createLogDirectory();
	}
	
	static function isEnabled()
	{
		return Settings::getValue("activity_tracker", "track_activity");
	}
	
	static function createLogDirectory()
	{
		if (ActivityTracker::isEnabled())
		{
			$logDirectory = Settings::getValue("activity_tracker", "logging_directory");
			
			if (!file_exists($logDirectory))
			{
				mkdir($logDirectory);
			}
		}
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
		$trackingMode = Settings::getValue("activity_tracker", "google_analytics_version");
		
		$script = "";

		ob_start();
		
		if ($trackingMode == "Classic")
		{
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
		}
		else if ($trackingMode == "Universal")
		{
?>
<script type="text/javascript">

  (function(i,s,o,g,r,a,m){i['GoogleAnalyticsObject']=r;i[r]=i[r]||function(){
  (i[r].q=i[r].q||[]).push(arguments)},i[r].l=1*new Date();a=s.createElement(o),
  m=s.getElementsByTagName(o)[0];a.async=1;a.src=g;m.parentNode.insertBefore(a,m)
  })(window,document,'script','//www.google-analytics.com/analytics.js','ga');

  ga('create', '<?php echo $id ?>', 'auto');
  ga('send', 'pageview');

</script>
<?
		}
		else if ($trackingMode == "GA4")
		{
?>
<!-- Google tag (gtag.js) -->

<script async src=https://www.googletagmanager.com/gtag/js?id=<?php echo $id?>></script>

<script>

  window.dataLayer = window.dataLayer || [];

  function gtag(){dataLayer.push(arguments);}

  gtag('js', new Date());
  gtag('config', '<?php echo $id?>');

</script>
<?
		}
		
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
	

	static function getBrowser($session)
	{
		$browser = get_browser($session->user_agent);
		return $browser ? "{$browser->parent} ({$browser->platform})" : "N/A";
	}	

	/**
	 * Loads activity records from a log file
	 * @param UserActivitySession $session the session to be loaded
	 * @return multitype:UserActivity
	 */
	static function loadSession($session)
	{
		$log = ActivityTracker::getLogFile($session->session_identifier, date("Y-m-d", strtotime($session->session_start)));
		
		trace("Opening session log $log", 3);
		
		$fp = fopen($log, "r");	
		
		$records = array();
		
		while($json = fgets($fp))
		{
			$activity = new UserActivity();
			$activity->fromJSON($json);
			$records[] = $activity;
		}

		fclose($fp);
		return $records;
	}
	
	/**
	 * Migrates data from database table (bad!) to session log files (good!)
	 */
	static function migrateData($session)
	{
		$records = Query::create(UserActivity, "WHERE user_id=:u AND session_id=:s ORDER BY activity_id")
								->bind(":u", $session->user_id, ":s", $session->session_id)
								->filter(new ExclusionFilter("response_time")) // Not captured in previous version
								->execute();
		
		$start = date("Y-m-d", strtotime($session->session_start));
		
		$log = ActivityTracker::getLogFile($session->session_id, $start);

		if (file_exists($log)) return;
		
		$userSession = new UserActivitySession();
		$userSession->session_identifier = $session->session_id;
		$userSession->user_id = $session->user_id;
		$userSession->session_start = $session->session_start;
		$userSession->session_end = $session->session_end;
		$userSession->feedback_count = $session->num_feedback;
		
		$pageViews = 0;
		$actionCount = 0;
		$requestCount = 0;
		
		foreach($records as $record)
		{
			error_log($record->toJSON()."\n", 3, $log);
			
			$requestCount++;
			if (startsWith("/action", $record->uri))
			{
				$actionCount++;
			}
			else
			{
				$pageViews++;
			}			
		}
		
		$userSession->page_views = $pageViews;
		$userSession->action_count = $actionCount;
		$userSession->requestCount = $requestCount;
		$userSession->save();
	}	
}

class ActivityReportFilterHelper
{
	function __construct()
	{
		
	}
	
	function setup()
	{
		$session = new UserActivitySession();
		$session->filter = new InclusionFilter("session_start", "feedback_user");
		$session->overrideFieldType("feedback_user", Boolean);
		$session->fromGET();
		
		$filterForm = new FilterForm($session);
		$filterForm->hide("session_end");
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
		return "feedback_count > 0";
	}
}

class SessionDetailsHelper
{
	var $showImages = false;
	
	function __construct()
	{
		$this->showImages = checkNumeric($_GET["showImages"]);		
	}
	
	function filterImages($record)
	{
		if (!$this->showImages && startsWith($record->uri, "/action/image/")) return false;
		return "";
	}
}
/**@}*/?>