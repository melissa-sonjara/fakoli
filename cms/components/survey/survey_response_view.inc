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
	
		$validate->ValidateView();
		if($validate->error > 0)
		{
			echo $this->response->getRequiredText($validate->error, true);
		}		
		$questionnaire = new QuestionnaireForm($this->response, true);
		echo "<h3>". $this->response->Questionnaire()->title . "</h3>\n";
		$questionnaire->drawView();	
	}
}
?>