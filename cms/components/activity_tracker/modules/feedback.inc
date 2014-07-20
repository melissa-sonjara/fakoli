<?php
Fakoli::using("settings");

$enabled = Settings::getValue("activity_tracker", "enable_feedback");
$require_login = Settings::getValue("activity_tracker", "require_login");

if ($enabled && ($user || !$require_login))
{
	
$scenario_title = Settings::getValue("activity_tracker", "feedback_title");
$thank_you = Settings::getValue("activity_tracker", "confirmation_message");

?>
 <div id="feedback_closed"><?echo $scenario_title?></div>
 <div id="feedback_open" style="display: none"><span id="feedback_open_title"><?echo $scenario_title?></span>
  <a href="#" id="feedback_send_button" class="button">Send</a>
  <textarea id="feedback_textarea" class="fixed-size"></textarea>
  <div id="feedback_thanks"><?echo $thank_you?></div>
 </div>
<?
}
?>