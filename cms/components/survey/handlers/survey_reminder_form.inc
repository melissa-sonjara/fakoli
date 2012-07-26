<?php
/**************************************************************

Copyright (c) 2007,2008 Sonjara, Inc

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
 * Description: Create a list of survey request recipients
 * that have not yet responded. Send an email reminder.
 *
 * @author: Janice Gallant for Sonjara, Inc.
 *
 * date: 10/21/2010
 */

Fakoli::using("survey", "email");
Fakoli::usingFeature("auto_form");

$survey_id = checkNumeric($_GET["survey_id"]);
$rtn = checkNumeric($_GET["rtn"]);

if(!$survey_id)
	ajaxReturn("unknown survey");

$survey = new Survey($survey_id);

if(!$survey->isAuthor())
	ajaxReturn("insufficient privileges to send reminder");

if($survey->isClosed())
	ajaxReturn("unable to send reminders - survey is closed");


$nonResponsives = $survey->getNonResponders();
$survey->recipients = formatItems($nonResponsives, "{email}", ", ");

$survey->filter = new InclusionFilter("title", "recipients", "sender_email", "message");
$form = new AutoForm($survey, "POST", "/action/survey/survey_reminder_form?survey_id=$survey_id&msg=$msg");
$form->ajaxSubmit("function(result) {new Survey().surveyReminderFormResult(result);}", "function() {\$('{$form->id}_error').set('text','Failed to communicate with server'); }");

$form->alias("recipients", "Recipients who have not yet responded", "sender_email", "From");
$form->getRenderer("message")->rows = 10;
$form->submitLabel = "Send Reminders";
$form->annotate("message", "You can use the email template message as is or customize it for this reminder.");
$form->readOnly("sender_email", "recipients", "title");
$form->required("message");
$form->button("Cancel", "new Survey().closeDialog()", null, true);

if($rtn)
{
	$form->readOnlyForm = true;
	$msg = ($rtn == 1) ? "<b>Reminders have been sent.</b>" : "<warning>Reminders could not be sent.</warning>";
}

if ($method == "POST")
{
	// We don't want to save their reminder message
	$form->msg = $form->validator->validate();
	if(!$form->msg)
	{
		$rtn = sendReminderEmails($form, $nonResponsives);
		ajaxReturn($rtn);
	}
	else
	ajaxReturn($form->msg);
}

echo $form->writeScript();

echo $msg;
$form->drawForm();


function sendReminderEmails($form, $nonResponsives)
{
	global $config;
	global $_POST;
	$survey = $form->data;

	$sender_email = ($survey->sender_email) ? $survey->sender_email : $survey->getSenderEmail();
	$subject = Settings::getValue("survey", "email_subject");
	$message_footer = Settings::getValue("survey", "message_footer");

	$message = $_POST["message"] . $message_footer;

	if(count($nonResponsives) > 0)
	{
		foreach($nonResponsives as $response)
		{
			$sent = SurveyManager::sendOneSurveyEmail($survey, $subject, $response, $message, $sender_email);
		}
	}

	return ($sent) ? "1" : 2;
}
?>