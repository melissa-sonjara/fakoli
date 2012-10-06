<?php
Fakoli::using("questionnaire", "svg_charts");

/**
 * Draws a graphical view of questionnaire/survey responses
 * 
 */
class QuestionnaireGraphicalView
{
	var $mgr;
	var $emptyMessage = "There are no results to display.";

	/**
	 * 
	 * @param $mgr obj of class QuestionnaireResultsManager or SurveyResultsManager 
	 * or a class with the following methods:
	 * getQuestions
	 * getAnswers - aggregated
	 */
	function QuestionnaireGraphicalView($mgr)
	{
		$this->mgr = $mgr;
	}

	function writeScript()
	{
		return "";
	}

	function drawView()
	{
		$questions = $this->mgr->getQuestions();
		$answers = $this->mgr->getAnswers();
		
		if(count($questions) == 0  || $this->mgr->getResponseCount() == 0)
		{
			echo "<p><em>{$this->emptyMessage}</em></p>";
			return;
		}
		
		$pk = $this->mgr->getQuestionKey();
		$answers = count($answers) ? regroupList($answers, $pk) : array();
		
		foreach($questions as $question)
		{
			$qAnswers = array();
			
			if(array_key_exists($question->$pk, $answers))
			{
				$qAnswers = $answers[$question->$pk];
				if(!is_array($qAnswers)) $qAnswers = array($qAnswers);
			}	
			$renderer = QuestionField::create($this->mgr->item, $question);
			echo "<h4>{$question->question_number}. {$question->question}</h4>\n";
			$renderer->drawGraph($this->mgr, $qAnswers);
		}
	}
}

