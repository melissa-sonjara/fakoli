<?php
Fakoli::using("settings");

$enabled = Settings::getValue("activity_tracker", "enable_feedback");

if ($enabled)
{
	
$scenario_title = Settings::getValue("activity_tracker", "feedback_title");

?>
 <div id="feedback_closed"><?echo $scenario_title?></div>
 <div id="feedback_open" style="display: none"><span id="feedback_open_title"><?echo $scenario_title?></span>
  <a href="#" id="feedback_send_button" class="button">Send</a>
  <textarea id="feedback_textarea" class="fixed-size"></textarea>
  <div id="feedback_thanks">Thank you, your feedback is appreciated!</div>
 </div>
<?
}
?>