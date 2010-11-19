<?php
class ValidateSurveyResponse
{
	var $response;
	var $error = 0;
	
	function ValidateSurveyResponse($response)
	{
		$this->response = $response;	
	}

	function Validate()
	{
		$questionnaire = new QuestionnaireForm($this->response, true);
		$msg = $questionnaire->validate();

		if($msg != "")
			redirect("/survey_response_form?response_id={$this->response->response_id}&error=1");
		
		return true;
	}
	
	function ValidateView()
	{
		$questionnaire = new QuestionnaireForm(new SurveyResponseManager($this->response), true);
		$msg = $questionnaire->validate();

		if($msg != "")
			$this->error = 1;
	}
}


?>