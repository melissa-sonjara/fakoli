<?php
/*
 * 
 * Title: survey_template_preview.inc
 * 
 * Description: Preview of a survey template. Admin users
 * can decide if it is ready to be published and used. 
 * 
 * @author: Janice Gallant for Sonjara, Inc.
 * 
 * date: 10/19/10
 * 
 */

Fakoli::using("survey");
Fakoli::usingFeature("auto_form");

require_once realpath(dirname(__FILE__)."/../../questionnaire/questionnaire_form.inc");
require_once realpath(dirname(__FILE__)."/../survey_template_view.inc");

$survey_template_id = checkNumeric($_GET["survey_template_id"]);

if (!$survey_template_id) 
	redirect("/survey_dashboard");

$surveyTemplate = new SurveyTemplate($survey_template_id);

$questionCount = queryValue(SurveyQuestionXref, "COUNT(1)", "WHERE survey_template_id=$survey_template_id");
$view = new SurveyTemplatePreview($surveyTemplate);

$surveyTemplate->filter = new InclusionFilter("status");
 
if($questionCount > 0)
{
	$form = new AutoForm($surveyTemplate);
	$form->submitLabel = ($surveyTemplate->isPublished()) ? "Revise this Template" : "Ready for Use";
	$form->onSaveComplete = updateStatus;	
	$form->hide("status");
}

$tabs = surveyTemplateTabs($survey_template_id);
$styles .= "<link href=\"/templates/css/tabs.css\" rel=\"stylesheet\"/>";

if ($method == "POST")
{
	if ($form AND $form->save())
	{
		//redirect("/survey_templates");
	}
}


if($form)
	$script .= $form->writeScript();

$script .= $view->writeScript();
$tabs->writeHTML();
?>
<div id="tab_border">
<?
$view->drawView();
if($form)
{
	$form->drawForm();
}
?>
</div>
<?php 
function updateStatus($form)
{
	global $_POST;
	$surveyTemplate = $form->data;
	
	$submit = $_POST["submit"];
	if($submit == "Ready for Use")
		$surveyTemplate->setPublished();
	else
		$surveyTemplate->setInProgress();
	
	redirect("/survey_templates");
}
?>