<?php


Fakoli::using("survey");
Fakoli::usingFeature("tree");

require_once realpath(dirname(__FILE__)."/../survey_template_view.inc");

$survey_template_id = checkNumeric($_GET["survey_template_id"]);

if(!$survey_template_id)
	redirect("/survey_templates");

$surveyTemplate = new SurveyTemplate($survey_template_id);
$redirect = "/survey_template_questions?survey_template_id=$survey_template_id";
$form = new SurveyQuestionSelectForm($surveyTemplate);
$form->button("Cancel", $redirect);

$tabs = surveyTemplateTabs($survey_template_id);
$tabs->page = "survey_template_questions";
$tabs->active = "Survey Template Questions";
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
<h3>Select Survey Template Questions</h3>
<?
$form->drawForm();
?></div>
