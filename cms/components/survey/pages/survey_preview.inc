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
 *
 * Title: survey_preview.inc
 *
 * Description: Preview of a survey response request. Lists each
 * email to be sent with email template, questionnaire, and
 * recipients.
 *
 * author: Janice Gallant for Sonjara, Inc.
 *
 * date: 5/13/10
 *
 */


Fakoli::using("questionnaire", "survey", "email");
Fakoli::usingFeature("auto_form");

$survey_id = checkNumeric($_GET["survey_id"]);

if (!$survey_id)
	redirect("survey_list");

$survey = new Survey($survey_id);

$sendMgr = new SurveySendManager($survey);
$view = new QuestionnaireSendView($sendMgr);

if(($survey->isEditable() && $sendMgr->isValid()))
{
	$survey->filter = new InclusionFilter("status", "start_date", "sender_email");
	$form = new AutoForm($survey);
	$form->hide("status", "start_date", "sender_email");
	$form->submitLabel = "Send Survey Emails";
	$form->button("Send Test Email to {$user->email}", "questionnaireSendMgr.sendTestEmail()", null, true);
	$form->submitLabel = $sendMgr->getSendSubmitLabel();
	$form->customSaveHandler = array($sendMgr, sendRequests);
}

$page->page_title = (!$survey->isSent()) ? "Preview " : "Manage ";
$page->page_title .= "Survey {$survey->title}";

$tabs = SurveyManager::surveyTabs($survey_id);

if ($method == "POST")
{
	$send_result = $form->save();
	{
		$tabs->queryString .= "&sent={$send_result}";
		$tabs->next();
	}
}

if($form) $script .= $form->writeScript();
$script .= $sendMgr->writeScript();
$script .= $view->writeScript();

$tabs->writeHTML();
?>
<div id="survey_tab_border">
<?php
echo "<p id='action_result' class='questionnaire_action_result' style='display: none'></p>\n";

// Next page link, set page title; set info text based on status;
// progress bar
echo $sendMgr->getSendPageHeading($tabs);

if($form)
{
	$form->drawForm();
}
else
{
	// if sent, draw manage links
	echo $sendMgr->getManageLinks();
}

$view->drawView();

?></div>