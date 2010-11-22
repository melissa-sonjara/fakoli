<?
/*
 * Description: provides the more generic questionnaire 
 * classes with the specifics of the survey datamodels and 
 * page identifiers. For example, lets the QuestionSet
 * class know the identifier name of the survey question form.
 * 
 * @author Janice Gallant for Sonjara, Inc.
 * 
 * 10/13/2010
 */
class SurveyManager
{
	var $item;
	
	/**
	 * Creates a new QuestionnaireManager object.
	 */
	function SurveyManager($survey)
	{
		$this->item = $survey;
	}
	
	function getQuestionKey()
	{
		return "survey_question_id";
	}
	
	function getAnswerKey()
	{
		return "survey_answer_id";
	}
	
	function getQuestions()
	{
		$questions = $this->item->Questions();
		if(count($questions) > 0)
		{
			$idx = 1;
			foreach($questions as $question)
			{
				$question->sort_order = $idx;
				$idx++;
			}
		}
		return $questions;
	}
	
	/**
	 * Returns the field name of the page identifier for 
	 * add/update of survey questions.
	 * 
	 * @return string
	 */
	function getQuestionForm()
	{
		return "survey_question_form?survey_id={$this->item->survey_id}&survey_question_id=";
	}
	
	function isEditable()
	{
		return $this->item->isEditable();
	}
	
	function getPrimaryKey()
	{
		return $this->item->getPrimaryKey();
	}
	
	/*
	 * Answers to a specific question
	 */
	function getAnswersOneQuestion($question_id)
	{
		return query(SurveyAnswer, "WHERE response_id IN (SELECT response_id FROM survey_response WHERE survey_id = {$this->item->survey_id}) AND survey_question_id={$survey_question_id}");
	}
	
	
	// The class that stores the answers
	function getAnswerClass()
	{
		return "SurveyAnswer";
	}
	
	
	function getAnswers()
	{
		return $this->item->Answers();
	}
}


class SurveyTemplateManager extends SurveyManager
{
	function SurveyTemplateManager($surveyTemplate)
	{
		$this->item = $surveyTemplate;		
	}

	/**
	 * Returns the field name of the page identifier for 
	 * add/update of survey questions.
	 * 
	 * @return string
	 */
	function getQuestionForm()
	{
		return "survey_template_question_form?survey_template_id={$this->item->survey_template_id}&survey_question_id=";
	}
		
	function isEditable()
	{
		return true;
	}
	
	function getAnswers()
	{
		return array();
	}
}


class SurveyResponseManager extends SurveyManager
{
	var $item;
	
	function SurveyResponseManager($response)
	{
		$this->item = $response;	
	}
	
	function getQuestions()
	{
		return $this->item->Questions();
	}
	
	function getAnswers()
	{
		return $this->item->Answers();
	}
	

	
}