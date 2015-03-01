<?php
/**************************************************************

 Copyright (c) 2010 Sonjara, Inc

 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:

 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.

 Except as contained in this notice, the name(s) of the above 
 copyright holders shall not be used in advertising or otherwise 
 to promote the sale, use or other dealings in this Software 
 without prior written authorization.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.

*****************************************************************/

Fakoli::using("email");
Fakoli::usingFeature("auto_form");

$menu_item = "Send Test Email";

$email = new Email();
$email->sender_email = $config["email_from"];
$email->subject = "Test message from {$config["site_name"]}";
$email->message = "Test message.";

$form = new AutoForm($email);
$form->submitLabel = "Send Email";
$form->readonly("sender_email");
$form->hide("sender_name");
$form->annotate("recipient", "Provide one or more recipient email address, separated by commas");
$form->alias("recipient", "Recipient(s)");

if ($method == "POST")
{
	$email->fromPOST();
	
	$handler = new EmailHandler($email->recipient, $email->subject, $email->message, $config["email_from"]);
	$handler->send();
}

$script .= $form->writeScript();
$form->drawForm();
?>