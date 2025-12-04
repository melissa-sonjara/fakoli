<?php
Fakoli::using("error_log", "user", "activity_tracker");
Fakoli::usingFeature("auto_form");

$menu_item = "Error Log";

$error_log_id = checkNumeric($_GET["error_log_id"]);

$error = new ErrorLog($error_log_id);

$tracking = Settings::getValue("activity_tracker", "track_activity");

if ($tracking)
{
	$sessions = UserActivitySession::getSessions($error->session);
	$sid = "";
	foreach($sessions as $s)
	{
		$sid .= "<a href='session_details?session_id={$s->session_id}'>{$error->session}</a><br/>";
	}
}
else
{
	$sid = $error->session;
}
?>
<b>Error encountered by <?echo $error->getUserFullName()?> on <?echo $error->timestamp?>.</b>
<p><b>Session ID: <?echo $sid?></b></p>
<p><b>URI:</b> <?echo $error->uri?>, <b>Referer:</b> <?echo $error->referer?></p> 
<p><?echo $error->message?></p>
<p><em><?echo $error->file?></em> Line <?echo $error->line?></p>
<h4>Stack Trace</h4>
<p><?echo formatAsHTML($error->details)?></p>
<br/>
<br/>
<a class='button' href='error_log'>&laquo; Back to Error Log</a>
