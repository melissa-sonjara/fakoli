<?php
Fakoli::using("email");

if (!checkRole("admin"))
{
	Fakoli::end("Permission Denied");
}

$email_log_id = checkNumeric($_GET["email_log_id"]);

if (!$email_log_id)
{
	Fakoli::end("Missing Parameter");
}

$msg = new EmailLog($email_log_id);

echo $msg->format("<pre>{message:xml}</pre>");
