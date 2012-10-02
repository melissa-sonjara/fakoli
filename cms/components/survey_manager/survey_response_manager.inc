<?php

class SurveyResponseView
{
	var $response;

	function SurveyResponseView($response)
	{
		$this->response = $response;
	}

	function drawView()
	{
		$validate = new ValidateSurveyResponse($this->response);

		if(!$validate->ValidateView())
		$validate->warning();
			
		$survey = new QuestionnaireForm(new SurveyResponseManager($this->response), true);
		echo "<h3>". $this->survey->title . "</h3>\n";
		$survey->drawView();
	}
}
