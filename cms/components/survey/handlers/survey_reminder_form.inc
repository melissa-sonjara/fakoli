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

Fakoli::using("survey", "questionnaire", "email", "settings");
Fakoli::usingFeature("auto_form");

$survey_id = checkNumeric($_GET["survey_id"]);
$dashboard = checkNumeric($_GET["dashboard"]);

if(!$survey_id)
	Fakoli::end("Survey Reminder Form - unknown survey");

$survey = new Survey($survey_id);

if(!$survey->isAuthor())
	Fakoli::end("insufficient privileges to send reminder");

if($survey->isClosed())
	Fakoli::end("unable to send reminders - survey is closed");

$mgr = new SurveySendManager($survey);
$form = $mgr->buildReminderForm($dashboard);

if ($method == "POST")
{
	$survey =& $form->data;
	$survey->fromPOST();
	
	$form->msg = $form->validator->validate();
	if(!$form->msg)
	{
		$rtn = $mgr->sendReminderEmails();
		$sent = ($rtn) ? "OK" : "FAIL";
		$send_result = ($rtn) ? questionnaire_reminder_sent : questionnaire_reminder_failed;
		Fakoli::end("{$sent}|". $mgr->getInfoMsg($send_result, $dashboard));
	}
	else
	{
		Fakoli::end($form->msg);
	}
}

echo $form->writeScript();

echo "<h2>" . $form->data->get("nonresponsive_count") ." recipients have not yet responded.</h2>\n";
$form->drawForm();
?>