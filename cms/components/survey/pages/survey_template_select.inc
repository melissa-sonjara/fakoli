<?php
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

require_once realpath(dirname(__FILE__)."/../survey_template_view.inc");

$surveyTemplates = query(SurveyTemplate, "ORDER BY title");
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
//$script .= SurveyTemplateViewDialog::writeScript();
$styles .= "<link href=\"/templates/css/table.css\" rel=\"stylesheet\"/>";
$styles .= "<link href=\"/templates/css/survey.css\" rel=\"stylesheet\"/>";

?>
<form method="POST" action="" id="survey_template_select_form">
<div id='survey_table'>
<br/>
<h3>Survey Templates</h3>
<?php
$templateTable->drawView();
?>
<br>
<h3 style='padding-bottom:0px; margin-bottom: 0px;'>Your Surveys</h3>
<?php
$userSurveyTable->drawView();
?>
<br/><input type="submit" name="submit" class="button" value="Select Survey Template"/>
</div>
</form>
<?php

function getSurveyTemplateTable($surveyTemplates)
{
	$table = new DataListView($surveyTemplates, "surveyTemplates");
	$table->column("Title", "<input style='border: none' type='radio' name='survey_template_id' value='{survey_template_id}'>{title}</input>", false, "width: 30%")
			->column("Introduction", "{introduction}", false, "width: 30%")
			->column("View", getTemplateView, false, "width: 3%");
	
	$table->emptyMessage = "There are no survey templates defined.";
	$table->sortable = false;
	$table->pageSize = 0;
	$table->filter = false;
	$table->cssStyle = "width: 80%";
	$table->paginator = false;

	return $table;
}

function getUserSurveyTable($userSurveys)
{
	$table = new DataListView($userSurveys, "userSurveys");
	$table->column("Title", "<input style='border: none' type='radio' name='survey_id' value='{survey_id}'>{title}</input>", false, "width: 30%")
			->column("Introduction", "{introduction}", false, "width: 30%")
			->column("View", getSurveyView, false, "width: 3%");
	
	$table->emptyMessage = "You do not yet have any surveys.";
	$table->sortable = false;
	$table->pageSize = 0;
	$table->filter = false;
	$table->cssStyle = "width: 80%";
	$table->paginator = false;
	
	return $table;
	
}

function getTemplateView($surveyTemplate)
{
	return "<a class=\"button\" href=\"javascript:modalPopup('Survey Template', '/survey_template_preview_popup?survey_template_id={$surveyTemplate->survey_template_id}', '400px', 'auto');\">View</a>\n";
}

function getSurveyView($survey)
{
	return "<a class=\"button\" href=\"javascript:modalPopup('Survey', '/survey_template_preview_popup?survey_id={$survey->survey_id}', '400px', 'auto');\">View</a>\n";
	
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
	$survey->sender_email = $surveySettings->sender_email;
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