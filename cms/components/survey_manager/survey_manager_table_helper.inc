<?php

class SurveyQuestionTableHelper
{
	function SurveyQuestionTableHelper()
	{
		
	}
	
	/**
	 * 
	 * @param $obj - instance of a survey question xref class or question class
	 */
	function formatOptions($obj)
	{
		if(!$obj->hasField("question_type_id") && $obj->hasRelation(Question))
		{
			$question = $obj->Question();
			if (!$question) return "";
		}
		else
		{
			$question = $obj;
		}
		
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


	/**
	 * 
	 * @param $obj - instance of a survey question xref class or question class
	 */
	function formatRequired($obj)
	{
		if(!$obj->hasField("question_type_id") && $obj->hasRelation(Question))
		{
			$question = $obj->Question();
			if (!$question) return "";
		}
		else
		{
			$question = $obj;
		}
	
		if($question->isCheckList())
			return ($question->required > 0) ? $question->required : "No";
		else
			return ($question->required == 1) ? "Yes" : "No";
	}

	static function formatQuestionNameInputField($question)
	{
		if(!$question->question_name)
			$question->question_name = substr($question->question, 0, 79);

		$pk = $question->getPrimaryKey();
		return $question->format("<input type='text' name=\"question_{$pk}\"
		value=\"{question_name}\" size=\"40\"/>");	
	}
}
?>