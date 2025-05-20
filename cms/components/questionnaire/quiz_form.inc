<?php
/**\addtogroup questionnaire
 * @{
 */


Fakoli::using("questionnaire");
Fakoli::usingFile("/cms/components/questionnaire/questionnaire_form.inc");


class QuizForm extends QuestionnaireForm
{
	var $isCompleted = false;
	var $correctCount = 0;
	var $questionCount;
	
	function __construct($mgr)
	{
		parent::__construct($mgr);
		$this->id = "Quiz_form";
		$this->checkCompleted();
		$this->questionCount = count($this->questions);
		$this->gradeQuiz();
	}
	
	function writeHTML()
	{
		if(count($this->questions) == 0)
		{
			echo "There are no questions.<br/>";
			return;
		}

		if ($this->msg)
		{
			echo "<tr>\n <td colspan='2'><span class='error'>{$this->msg}</span</td></tr>\n";
			$this->msg = "";
			echo "</span>\n";
		}
	
		echo "<dl>\n";
		
		foreach($this->questions as $question)
		{
			$this->writeOneQuestion($question);
		}

		echo "</dl>\n";
	}

	function writeOneQuestion($question)
	{
		$qPk = $question->getID();
		$num = ($question->question->question_number) ? $question->question->question_number : $this->mgr->getQuestionNumber();

		$class = $this->getQuestionClass($question);
		
		echo " <dt id='dt_question_{$qPk}' class='{$class}'><label for='question_{$qPk}'>";
		
		if (!$question->skipNumbering)
		{
			echo "<b>{$num}.</b> ";
		}
		
		echo "{$question->question->question}</label></dt>\n";

		echo " <dd>";

		$question->writeHTML();
	
		echo " <br/></dd>";
	}
	
	function checkCompleted()
	{
		foreach($this->questions as $question)
		{	
			if(!$question->answer->value) 
			{
				$this->isCompleted = false;
				return;
			}		
		}
		$this->isCompleted = true;
	}
	
	function passed()
	{
		if($this->correctCount == $this->questionCount)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	
	function gradeQuiz()
	{
		foreach($this->questions as $question)
		{	
			if($question->answer->value && $question->answer->value == $question->question->answer)
			{
				$this->correctCount ++;
			}
		}
	}
	
	function formatMissed()
	{
		$count = $this->questionCount - $this->correctCount;
		
		return ($count > 1) ? "$count questions" : "$count question";
	}
	
	function isCompleted()
	{
		return ($this->isCompleted) ? true : false;
	}
	
	/**
	 * 
	 * @param obj $question object of QuestionField
	 */
	function getQuestionClass($question)
	{
		if(!$question->question->hasField("answer") || !$this->isCompleted() || !$question->answer->value)
		{
			return "";
		}	
		
		return ($question->answer->value == $question->question->answer) ? "correct_answer" : "incorrect_answer";
	}	
	
}
/**@}*/?>
