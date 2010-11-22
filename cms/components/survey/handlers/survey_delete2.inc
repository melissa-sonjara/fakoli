<?php
/*
 * survey_delete2.inc
 * 
 * Description: Nondisplay script to delete a survey.
 * Hidden file for developers only.
 *  
 * author: Janice Gallant for Sonjara, Inc.
 * 
 * date: 6/4/10
 */

Fakoli::using("survey");
Fakoli::assertRole("admin");

$survey_id = checkNumeric($_GET["survey_id"]);

if(!$survey_id)
	redirect("/survey_list");
	
$survey = new Survey($survey_id);

$responses = $survey->Responses();

if(count($responses) > 0)
{
	foreach($responses as $response)
	{
		$answers = $response->Answers();
		if(count($answers) > 0)
		{
			foreach($answers as $answer)
				$answer->delete();
		}
		$response->delete();
	}
}

$survey->delete();

redirect("/survey_list");