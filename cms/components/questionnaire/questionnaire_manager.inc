<?
/*
 * Description: provides the more generic questionnaire 
 * classes with the specifics of the questionnaire datamodels and 
 * page identifiers. For example, lets the QuestionSet
 * class know the identifier name of the question form.
 * 
 * @author Janice Gallant for Sonjara, Inc.
 * 
 * 10/13/2010
 */
class QuestionnaireManager
{
	var $item;
	
	/**
	 * Creates a new QuestionnaireManager object.
	 */
	function QuestionnaireManager($questionnaire_id)
	{
		$this->item = new Questionnaire($questionnaire_id);
	}
	
	function getQuestions()
	{
		return $this->item->Questions("ORDER BY question_number");
	}
	
	/**
	 * Returns the field name of the page identifier for 
	 * add/update of survey questions.
	 * 
	 * @return string
	 */
	function getQuestionForm()
	{
		return "question_form";
	}
	
	function getQuestionKey()
	{
		return "question_id";
	}
	
	function getAnswerKey()
	{
		return "answer_id";
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
		return $this->item->getNotEditableHTML();	
	}
	
	function getAnswers()
	{
		return $this->item->Answers();
	}
	
	function getQuestionDeleteHandler()
	{
		return "/action/questionnaire/question_delete";
	}
	
	function drawQuestionDeleteButton($pk)
	{
		$confirm = "Are you sure you want to remove this question?";	
		$url = "/action/questionnaire/question_delete?questionnaire_id={$this->item->questionnaire_id}&question_id=$pk";
		QuestionSet::drawOneButton($url, "Delete", $confirm);
	}
	
	
	function getOrderField()
	{
		return "question_number";
	}
	
	function reOrderQuestions()
	{
		global $_POST;
		
		// reorder
		foreach($_POST as $name => $value)
		{
			if (!strncmp($name, "question_", 9))
			{
				$id = substr($name, 9);
				$question = new Question();
				$question->filter = new InclusionFilter("question_id", "question_number");	
				$question->questionnaire_id = $this->item->questionnaire_id;
				$question->question_id = $id;
				$question->question_number = $value;
				$question->save();
			}
		}
		// renumber
		$questionnaire = new Questionnaire($this->item->questionnaire_id);
		$questions = $questionnaire->Questions("ORDER BY question_number");
		$value = 1;
	
		foreach($questions as $question)
		{
			$question->filter = new InclusionFilter("question_number");
			$question->question_number = $value;
			$question->save();
			$value++;
		}

	}
	
	static function upgradeComponent()
	{
		$mgr = new QuestionnaireUpgradeManager();
		$mgr->upgrade();
	}	
}
?>