<?php
Fakoli::using("activity_tracker");

if (!$user) ajaxReturn("0");

$msg = $_POST["feedback"];

$feedback = new UserFeedback();

$feedback->user_id 		= $user->user_id;
$feedback->feedback		= $msg;
$feedback->created_date = now();
$feedback->session		= session_id();

$feedback->save();

ajaxReturn("1");
?>