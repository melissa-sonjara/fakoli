<?php
 /* 
 * Description: Nondisplay script to copy a questionnaire. User
 * may wish to copy a questionnaire when the type of editing
 * desired is beyond what is allowed for questionnaires that
 * have been answered.
 * 
 * author: Janice Gallant for Sonjara, Inc.
 * 
 * date: 5/18/10
 */

Fakoli::using("survey");

$survey_id = checkNumeric($_GET["survey_id"]);

if(!$survey_id)
	ajaxReturn("Clone Survey - unknown survey");
	
$survey = new Survey($survey_id);
$clone = clone $survey;
$clone->survey_id = 0;

$pk = $user->getPrimaryKey();
$clone->user_id = $user->$pk;
$clone->status = "not sent";

do
{
	$clone->title = "Copy of {$clone->title}";
} while ($clone->queryValue("COUNT(1)", "WHERE title = '{$clone->title}'") > 0);
		
$clone->save();

$xrefs = $survey->SurveyQuestionXrefs("ORDER BY sort_order");
if(count($xrefs) > 0)
{
	foreach($xrefs as $xref)
	{
		$copyXref = new SurveyQuestionXref();	 
		$copyXref->survey_id = $clone->survey_id;
		$copyXref->survey_question_id = $xref->survey_question_id;
		$copyXref->sort_order = $xref->sort_order;
		$copyXref->save();
	}            
}

ajaxReturn("OK");
?>