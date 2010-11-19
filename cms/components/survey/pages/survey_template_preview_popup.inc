<?php
/**************************************************************
 * 
 * Title: survey_template_preview_popup.inc
 * 
 * Description: Displays view of a questionnaire in a popup.
 * 
 * author: Janice Gallant for Sonjara, Inc.
 * 
 * date: 5/27/2010
 * 
***************************************************************/

Fakoli::using("survey");

$survey_template_id = checkNumeric($_GET["survey_template_id"]);
$survey_id = checkNumeric($_GET["survey_id"]);

if($survey_template_id)
	$surveyTemplate = new Survey($survey_template_id);
elseif($survey_id)
	$surveyTemplate = new Survey($survey_id);

if($surveyTemplate)
{
	$view = new QuestionnairePreview($surveyTemplate);
?>
<h3><?echo $surveyTemplate->title ?></h3>
<?
	$view->writeHTML();
}
else
{
	echo "Error: invalid survey_template_id.";
}
?>