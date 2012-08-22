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
 * survey_form.inc
 *
 * Description: Link an email template to a specific questionnairesurvey
 * and set of recipients. Provide a button for user to send the emails.
 *
 * author: Janice Gallant for Sonjara, Inc.
 *
 * date: 5/13/10
 */


Fakoli::using("questionnaire", "survey", "survey_manager");
Fakoli::usingFeature("auto_form");

$survey_id = checkNumeric($_GET["survey_id"]);

$title = "Survey Request";

$survey = new Survey();

$page->page_title = "Survey Introduction";

if($survey_id)
{
	$survey->load($survey_id);
	$page->page_title .= " for {$survey->title}";
}
else
{
	$pk = $user->getPrimaryKey();
	$survey->user_id = $user->$pk;
	$survey->sender_email = $config["email_from"];
	$survey->status = "not sent";
}

$survey->filter = new InclusionFilter("title", "introduction", "confirmation_message", "start_date", "end_date", "user_id", "status", "allow_anonymous_responses");
$form = new AutoForm($survey);
$form->required("title");
$form->hide("user_id", "status");
$form->unique("title");
$form->submitLabel = "Save";
$form->annotate("end_date", "date on which survey closes to respondents");
$form->getRenderer("introduction")->rows = 8;
$form->alias("confirmation_message", "Confirmation Message for Respondents",
	"introduction", "Introduction for Respondents");

if($survey->isClosed())
{
	$form->readOnly("end_date");
}
else if($survey->isOpen())
{
	$form->readOnly("start_date");
}

$form->allowDelete = $survey->allowDelete();

$form->layout = new UngroupedFormLayout($form);
$form->group("all", "title", "start_date", "end_date", "introduction");

$tabs = SurveyManager::surveyTabs($survey_id);
$script .= $form->writeScript();

if ($method == "POST")
{
	if ($form->save())
	{
		if ($form->deleted)
		{
			redirect("survey_dashboard");
		}
		
		$tabs->queryString = "survey_id={$form->data->survey_id}";
		$tabs->next();
	}
}

$tabs->writeHTML();

if(!$survey_id)
{
?>
<div id="survey_tab_border">
<?php 
}
else
{
?>
<div id="survey_tab_border">
<button id="next_page_button" class="button"
	onclick="go('<?echo $tabs->getNextPage() ?>')">Next Page &raquo;</button>
<?php
}
?>
<h3>Survey Introduction</h3>
<?
$form->drawForm();
?></div>
<?
if($survey_id)
{
?></div><?php
}
?>