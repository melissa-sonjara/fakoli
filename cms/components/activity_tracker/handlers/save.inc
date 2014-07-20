<?php
Fakoli::using("activity_tracker", "email");

$enabled = Settings::getValue("activity_tracker", "enable_feedback");
$require_login = Settings::getValue("activity_tracker", "require_login");
$sendEmail = Settings::getValue("activity_tracker", "send_email");

if (!$enabled || (!$user && $require_login)) Fakoli::end("0");

$msg = $_POST["feedback"];

$feedback = new UserFeedback();

$feedback->user_id 		= $user ? $user->user_id : 0;
$feedback->feedback		= $msg;
$feedback->created_date = now();
$feedback->referer = $_SERVER['HTTP_REFERER'];
$feedback->session		= session_id();

$feedback->save();

if ($sendEmail)
{
	sendEmailUsingEmailManager($feedback, "feedback_notification");
}

Fakoli::end("1");
?>