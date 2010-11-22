<?php
/*
 * survey_delete.inc
 * 
 * Description: Nondisplay script to delete a survey.
 *  
 * author: Janice Gallant for Sonjara, Inc.
 * 
 * date: 5/18/10
 */

Fakoli::using("survey");

$survey_id = checkNumeric($_GET["survey_id"]);
$return = checkNumeric($_GET["return"]);

if(!$survey_id)
	redirect("/survey_list");

$redirect = (!$return) ? "/survey_list" : "/survey_manage?survey_id={$survey_id}";
	
$survey = new Survey($survey_id);

if(!$survey->isAuthor() OR $survey->status == survey_open)
	redirect($redirect);
	
if($survey->status == survey_not_sent)
	$survey->delete();
elseif($survey->status == survey_closed)
{
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
	$survey->filter = new InclusionFilter("deleted");
	$survey->deleted = true;
	$survey->save();
}	


redirect($redirect);