<?php


Fakoli::using("survey");
Fakoli::usingFeature("tree");

require_once realpath(dirname(__FILE__)."/../survey_template_view.inc");
require_once realpath(dirname(__FILE__)."/../survey_view.inc");

$survey_id = checkNumeric($_GET["survey_id"]);

if(!$survey_id)
	redirect("/survey_dashboard");

$redirect = "/survey_questions?survey_id=$survey_id";
$survey = new Survey($survey_id);
$form = new SurveyQuestionSelectForm($survey);
$form->button("Cancel", $redirect);

$tabs = surveyTabs($survey_id);
$tabs->page = "survey_questions";
$tabs->active = "Questions";

$styles .= "<link href=\"/templates/css/tabs.css\" rel=\"stylesheet\"/>";

if ($method == "POST")
{
	if ($form->save())
	{
		redirect($redirect);
	}
}

$script .= $form->writeScript();
$tabs->writeHTML();
?>
<div id="tab_border">
<h3>Select Survey Questions</h3>
<?
$form->drawForm();
?></div>
