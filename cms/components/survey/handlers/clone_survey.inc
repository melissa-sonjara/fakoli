<?php
Fakoli::using("survey", "questionnaire");

$survey_id = checkNumeric($_GET["survey_id"]);

if(!$survey_id)
	Fakoli::end("Clone Survey - unknown survey");
	
$src = new Survey($survey_id);

$dst = new Survey($survey_id);
$dst->survey_id = 0;
$dst->title = "Copy of {$src->title}";
$dst->status = "not sent";
$dst->save();

$questions = $src->Questions();

$ids = array();

foreach($questions as $question)
{
	$newQuestion = new SurveyQuestion();
	$newQuestion->copy($question);
	$newQuestion->survey_question_id = 0;
	$newQuestion->save();
	
	$ids[$question->survey_question_id] = $newQuestion->survey_question_id;
}

$xrefs = $src->SurveyQuestionXrefs();

foreach($xrefs as $xref)
{
	$newXref = new SurveyQuestionXref();
	$newXref->survey_id = $dst->survey_id;
	$newXref->survey_question_id = $ids[$xref->survey_question_id];
	$newXref->sort_order = $xref->sort_order;
	$newXref->save();
}

Fakoli::end("OK");
?>