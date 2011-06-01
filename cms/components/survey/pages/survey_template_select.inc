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
 * When a user wants to create a new survey, lets users select from the set of existing 
 * survey templates.
 * 
 * @author Janice Gallant for Sonjara, Inc.
 * 
 * 10/18/2010
 */

Fakoli::using("survey");
Fakoli::usingFeature("data_view");

$surveyTemplates = query(SurveyTemplate, "WHERE status = ". survey_template_published . " ORDER BY title");
$userSurveys = query(Survey, "WHERE user_id={$user->user_id}");

$templateTable = getSurveyTemplateTable($surveyTemplates);
$userSurveyTable = getUserSurveyTable($userSurveys);


if ($method == "POST")
{
	$survey = formSave();
	{
		redirect("/survey_form?survey_id={$survey->survey_id}");
	}
}

$script .= $templateTable->writeScript();
$script .= $userSurveyTable->writeScript();
$styles .= "<link href=\"/templates/css/table.css\" rel=\"stylesheet\"/>";
$styles .= "<link href=\"/templates/css/survey.css\" rel=\"stylesheet\"/>";

?>
<form method="POST" action="" id="survey_template_select_form">
<p>You can select a survey template as a basis for your survey or use one of your existing surveys. You will be able to change the introduction, message, and question set if you wish.</p>
<div id='survey_table'>
<br/>
<h3>Survey Templates</h3>
<?php
$templateTable->drawView();
?>
<br>
<input type="submit" name="submit" class="button" value="Select Survey Template"/>
<h3 style='padding-bottom:0px; margin-bottom: 0px;'>Your Surveys</h3>
<?php
$userSurveyTable->drawView();
?>
<br/><input type="submit" name="submit" class="button" value="Select My Survey as a Template"/>
</div>
</form>
<?php

function getSurveyTemplateTable($surveyTemplates)
{
	$table = new DataListView($surveyTemplates, "surveyTemplates");
	$table->column("Title", "<input style='border: none' type='radio' name='survey_template_id' value='{survey_template_id}'>{title}</input>", false, "width: 20%")
			->column("Introduction", "{introduction}", false, "width: 35%")
			->column("Email Message", "{message}", false, "width: 35%")
			->column("View", getTemplateView, false, "width: 3%");
	
	$table->emptyMessage = "There are no survey templates defined.";
	$table->sortable = false;
	$table->pageSize = 0;
	$table->filter = false;
	$table->cssStyle = "width: 90%";
	$table->paginator = false;

	return $table;
}

function getUserSurveyTable($userSurveys)
{
	$table = new DataListView($userSurveys, "userSurveys");
	$table->column("Title", "<input style='border: none' type='radio' name='survey_id' value='{survey_id}'>{title}</input>", false, "width: 20%")
			->column("Dates", formatSurveyDates, false, "width: 10%")
			->column("Introduction", formatIntroduction, false, "width: 35%")
			->column("Email Message", formatMessage, false, "width: 35%")
			->column("View", getSurveyView, false, "width: 3%");
	
	$table->emptyMessage = "You do not yet have any surveys.";
	$table->sortable = false;
	$table->pageSize = 0;
	$table->filter = false;
	$table->cssStyle = "width: 90%";
	$table->paginator = false;
	
	return $table;
	
}


function formatIntroduction($surveyTemplate)
{
	return ellipsis($surveyTemplate->introduction, 80, true);	
}

function formatSurveyDates($survey)
{
	if(!$survey->start_date)
		$text = $survey->statusTypes[$survey->status] . "<br>";
	elseif($survey->start_date)
		$text .= "Opened: " . formatDateShort($survey->start_date) . "<br>";
		
	if($survey->end_date)
		$text .= "Closed: " . formatDateShort($survey->end_date);

	return $text;
}

function formatMessage($surveyTemplate)
{
	return ellipsis($surveyTemplate->message, 80, true);	
}

function getTemplateView($surveyTemplate)
{
	return "<a class=\"button\" href=\"javascript:modalPopup('Survey Template', '/survey_template_preview_popup?survey_template_id={$surveyTemplate->survey_template_id}', '700px', 'auto');\">View</a>\n";
}

function getSurveyView($survey)
{
	return "<a class=\"button\" href=\"javascript:modalPopup('Survey', '/survey_template_preview_popup?survey_id={$survey->survey_id}', '700px', 'auto');\">View</a>\n";
	
}

function formSave()
{
	global $_POST;
	global $user;
	
	$surveySettings = SurveySetting::loadSettings();
	
	$survey_template_id = $_POST["survey_template_id"];
	$survey_id = $_POST["survey_id"];
	
	if(!$survey_template_id AND !$survey_id)
		redirect("/survey_dashboard");
		
	if($survey_template_id)
		$surveyTemplate = new SurveyTemplate($survey_template_id);
	elseif($survey_id)
		$surveyTemplate = new Survey($survey_id);
		
	$survey = new Survey();
	$fields = array("title", "introduction", "message");
	foreach($fields as $field)
		$survey->$field = $surveyTemplate->$field;
	$survey->sender_email = Survey::getSenderEmail();
	$survey->user_id = $user->user_id;
	$survey->save();
	
	$questions = $surveyTemplate->Questions();

	if(count($questions) > 0)
	{
		$idx = 1;
		foreach($questions as $question)
		{
			$xref = new SurveyQuestionXref();
			$xref->survey_id = $survey->survey_id;
			$xref->survey_question_id = $question->survey_question_id;
			$xref->sort_order = $idx;
			$xref->save();
			$idx++;
		}
	}
	return $survey;
}
?>