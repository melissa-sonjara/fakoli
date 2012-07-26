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

/*
 * Provides a modal dialog "Mail To" form as an alternative
 * for the often sluggish performance of a user's mail client.
 * 
 * @author Janice Gallant for Sonjara, Inc.
 * 
 * 4/15/2011
 */


Fakoli::using("email");
Fakoli::usingFeature("auto_form");

$to = mysql_escape_string($_GET["to"]);
$subject = mysql_escape_string($_GET["subject"]);
$message = mysql_escape_string($_GET["message"]);

$email = new EmailTemplate();

$email->recipients = ($to) ? $to : "";
$email->subject = ($subject) ? $subject : "";
$email->message = ($message) ? $message : "";

$form = new AutoForm($email);
$form = new AutoForm($email, "POST", "/action/email/mail_to?to=$to&subject=$subject&message=$message", "MailTo_form");

$form->hide("sender_email", "class_name", "name");
$form->ajaxSubmit("function(result) {new EmailManager().sendMailTo(result);}", "function() {\$('{$form->id}_error').set('text','Failed to communicate with server'); }");
$form->button("Cancel", "new EmailManager().closeMailToDialog()", null, true);
$form->submitLabel = "Send Email";
$form->validator->add(new EmailListValidator("recipients", "Recipients"));
$form->required("recipients", "subject", "message");
$form->override("message", $label = "Message", new TextFieldRenderer($form));
$form->getRenderer("message")->rows = 10;
$form->override("recipients", $label = "To", new StringFieldRenderer($form));

if ($method == "POST")
{
	$obj =& $form->data;
	$obj->fromPOST();
	$msg = $form->validator->validate();
	if ($msg == "")
	{
		$mgr = new EmailHandler($obj->recipients, $obj->subject, $obj->message, $user->email);
		$mgr->send();
		echo "OK";
		return;
	}
	else
	{
		echo $msg;
		return;
	}		
}

echo $form->writeScript();

$form->drawForm();
?>