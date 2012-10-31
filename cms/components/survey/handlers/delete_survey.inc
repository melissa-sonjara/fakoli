<?php
/*
 * Description: Deletes a survey and all its relations.
 *
 * author: Janice Gallant for Sonjara, Inc.
 *
 * date: 5/18/10
 */

Fakoli::using("survey", "questionnaire");

$survey_id = checkNumeric($_GET["survey_id"]);

if(!$survey_id)
	Fakoli::end("unknown survey");

$survey = new Survey($survey_id);

if(!$survey->isAuthor() OR $survey->status == "open")
	Fakoli::end("cannot delete survey");

if($survey->status == "not sent")
{
	SurveyManager::deleteSurvey($survey);
	// truly delete - function call just flags as deleted.
	$survey->delete();
}
elseif($survey->status == "closed")
{
	// sets deleted flag; deletes all responses
	SurveyManager::deleteSurvey($survey);
}

Fakoli::end("OK");