<?php
Fakoli::using("email");
Fakoli::usingFeature("auto_form");

$email = new Email();

$form = new AutoForm($email);
$form->submitLabel = "Send Email";

if ($method == "POST")
{
	$email->fromPOST();
	
	$handler = new EmailHandler($email->recipient, $email->subject, $email->message, $email->sender_email);
	$handler->send();
}

$script .= $form->writeScript();
$form->drawForm();
?>