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

Fakoli::using("survey", "survey_manager");
Fakoli::assertRole("admin");

$survey_id = checkNumeric($_GET["survey_id"]);

if(!$survey_id)
	"Error: survey not found<br>";

$survey = new Survey($survey_id);

if(SurveyManager::deleteSurvey($survey))
$survey->delete();
else
	"Error: Unable to delete survey {$survey->title}<br>";
