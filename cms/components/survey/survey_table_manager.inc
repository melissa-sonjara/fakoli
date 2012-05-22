<?php

class CMSSurveyTableManager
{
	function CMSSurveyTableManager()
	{

	}

	static function formatSurveyLink($survey)
	{
		$path = ($survey->status != "not sent") ? "survey_results" : "survey_form";
		return "<a href=\"/$path?survey_id={$survey->survey_id}\">{$survey->title}</a>";
	}

	static function formatSurveyStatus($survey)
	{
		return Survey::$statusTypes[$survey->status];
	}

	static function formatStartDate($survey)
	{
		$date = ($survey->start_date > 0) ? formatDateShort($survey->start_date) : '-- / -- / ----';
		return $date;
	}

	function formatSurveyQuestionLink($xref)
	{
		$question = $xref->Question();
		$editable = $question->isEditable();
		$isTemplateQuestion = $question->isTemplateQuestion();

		if(!$editable || $isTemplateQuestion)
			return $question->question;
		else
			return $xref->format("<a href=\"survey_question_form?survey_id={survey_id}&survey_question_id={survey_question_id}\">{Question.question}</a>");
	}

	function formatOptions($xref)
	{
		$question = $xref->Question();
		if (!$question) return "";
		
		if($question->isFreeText() || $question->isShortText())
		{
			$text .= "Character Limit: ";
			$text .=  ($question->char_limit) ? $question->char_limit : "None";
			$text .= "<br>";
		}
		else
		{
			$text =  preg_replace("/\n/", "; ", $question->options);
		}

		if($question->isFreeText())
			$text .= "Number of Rows: " . $question->num_rows . "<br>";
			
		return $text;
	}


	function formatRequired($xref)
	{
		$question = $xref->Question();
		if (!$question) return "";
		
		if($question->isCheckList())
			return ($question->required > 0) ? $question->required : "No";
		else
			return ($question->required == 1) ? "Yes" : "No";
	}

	static function formatQuestionNameInputField($question)
	{
		if(!$question->question_name)
			$question->question_name = substr($question->question, 0, 79);
			
		return $question->format("<input type='text' name=\"question_{survey_question_id}\"
		value=\"{question_name}\" size=\"40\"/>");	
	}

	static function formatResponseStatus($response)
	{
		$statusTitle = $response->statusTypes[$response->status];
		return $statusTitle;
	}


	static function formatResponseSubmitDate($response)
	{
		return ($response->last_modified > 0) ? formatDate($response->last_modified) : "N/A";
	}
}