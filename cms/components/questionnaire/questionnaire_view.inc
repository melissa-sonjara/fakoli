<?php

/**
 * Draw a view of the questionnaire from action handler from the
 * dashboard.
 * 
 * @author janice
 *
 */
class QuestionnaireView
{
	var $questionnaire;
	var $questionnaireView;
	
	function QuestionnaireView($questionnaire)
	{
		$this->questionnaire = $questionnaire;
		
		$this->questionnaireView = new QuestionnairePreview(new QuestionnaireCreateManager($questionnaire));
	}
	
	function writeScript()
	{
		return "";
	}
	
	function drawView()
	{
		$this->questionnaireView->drawView();
	}
}