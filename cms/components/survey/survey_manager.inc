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
	
	function getNotEditableHTML()
	{
		return "";	
	}

	function drawQuestionDeleteButton($pk)
	{
		$confirm = "Are you sure you want to remove this question?";	
		$url = "/action/survey/question_unlink?survey_id={$this->item->survey_id}&survey_question_id=$pk";
		QuestionSet::drawOneButton($url, "Remove", $confirm);
	}
	
	// The class that stores the answers
	function getAnswerClass()
	{
		return "SurveyAnswer";
	}
	
	function getOrderField()
	{
		return "sort_order";
	}
	
	function reOrderQuestions()
	{
		global $_POST;

		$xrefs = $this->item->SurveyQuestionXref("ORDER BY sort_order");
		if(count($xrefs) > 0)
			$indexedXrefs = reindexList($xrefs, "survey_question_id");
		else
			$indexedXrefs = array();
			
		// reorder
		foreach($_POST as $name => $value)
		{
			if (!strncmp($name, "question_", 9))
			{
				$id = substr($name, 9);
				$xref = $indexedXrefs[$id];
				if($xref)
				{
					$xref->sort_order = $value;
					$xref->save();
				}
			}
		}
		// renumber
		$value = 1;
	
		foreach($xrefs as $xref)
		{
			$xref->filter = new InclusionFilter("sort_order");
			$xref->sort_order = $value;
			$xref->save();
			$value++;
		}
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
	
	function getAnswers()
	{
		return $this->item->Answers();
	}
	

	
}