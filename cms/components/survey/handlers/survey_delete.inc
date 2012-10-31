<?php
/*
 * Description: Nondisplay script to delete a survey.
 *
 * Actually deletes the survey; not just flag as deleted.
 *
 * Hidden file for developers only.
 *
 * author: Janice Gallant for Sonjara, Inc.
 *
 * date: 6/4/10
 */

Fakoli::using("survey", "questionnaire");

$survey_id = checkNumeric($_GET["survey_id"]);

if(!$survey_id)
	Fakoli::end("unknown survey");

$survey = new Survey($survey_id);

if($survey->allowDelete())
{
	SurveyManager::deleteSurvey($survey);
	Fakoli::end("OK");
}
else
{
	Fakoli::end("delete not allowed for survey {$survey->title}");
}
?>