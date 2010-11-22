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

Fakoli::using("survey", "email", "program");
Fakoli::usingFeature("auto_form");

$survey_id = checkNumeric($_GET["survey_id"]);

if(!$survey_id)
	redirect("/survey_dashboard");
	
$survey = new EmailSurvey($survey_id);

if(!$survey->isAuthor())
	redirect("/survey_dashboard");
	
if($survey->isClosed())
	redirect("/survey_dashboard");
	
$nonResponsives = query(SurveyResponse, "WHERE survey_id = {$survey_id} AND status != ". response_submitted);
$survey->recipients = getNonResponsiveList($nonResponsives);
$survey->filter = new InclusionFilter("recipients", "email_from", "message");
$form = new AutoForm($survey);

$form->alias("recipients", "To", "email_from", "From");
$form->getRenderer("message")->rows = 10;
$form->submitLabel = "Send Reminders";
$form->annotate("message", "You can use the email template message as is or customize it for this reminder.");
$form->readOnly("email_from", "recipients");
$form->required("message");
$redirect = "/survey_results?survey_id=$survey_id";
$form->button("Cancel", $redirect);

$tabs = surveyTabs($survey_id);
$tabs->active = "Survey Results";
$tabs->page = "survey_results";

$menuView = new SurveyMenuView('survey_form');
$survey_top_nav = $menuView->getView();

if ($method == "POST")
{
	// We don't want to save their reminder message
	$form->msg = $form->validator->validate();
	if(!$form->msg)
	{
		$rtn = sendReminderEmails($form, $nonResponsives);
		$msg_id = ($rtn == true) ? 1 : 2;
		redirect($redirect . "&msg_id=$msg_id");
	}
}

$script .= $form->writeScript();
$tabs->writeHTML();
?>
<div id="tab_border">
<button id="next_page_button" class="button" onclick="go('<?echo $tabs->getNextPage() ?>')">Next Page &raquo;</button>
<h3>Send Survey Reminders</h3>
<div id="inner_border">
<?
	$form->drawForm();
?>
</div></div>
<?

function getNonResponsiveList($nonResponsives)
{
	if(count($nonResponsives) > 0)
	{
		$indexed = reindexList($nonResponsives, "email");
		$nonResponsiveList = implode(",", array_keys($indexed));
	}	
	
	return $nonResponsiveList;
}


function sendReminderEmails($form, $nonResponsives)
{
	global $config;
	global $_POST;
	
	$from = $config['survey_email'];
	
	$survey = $form->data;
	$message = $_POST["message"];
	$surveyEmailTemplate = $survey->EmailTemplate();
	
	if(count($nonResponsives) > 0)
	{
		foreach($nonResponsives as $response)
		{
			$sent = sendOneSurveyEmail($survey, $response, $surveyEmailTemplate);
		}
	}
	
	return $sent;
}
?>