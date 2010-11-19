<?php

class ValidateSurvey
{
	var $survey;
	var $error;
	
	function ValidateSurvey($survey)
	{
		$this->survey = $survey;
	}

	/* 
	 * When this validate function is called, the user
	 * is redirected to the page where they can fix errors
	 */
	function Validate()
	{
		$this->error = $this->validateQuestionnaire();
		if($this->error)
			redirect("/survey_questions?survey_id={$this->survey->survey_id}&error={$this->error}");
		
		$this->error = $this->validateEmail();
		if($this->error)
			redirect("/survey_email?survey_id={$this->survey->survey_id}&error={$this->error}");
		
		return true;
	}
	
	function validateQuestionnaire()
	{
		$questions = $this->survey->Questions();
		if(count($questions) == 0)
			$error = error_no_questions;
			
		return $error;
	}
		
	function validateEmail()
	{
		if(!$this->survey->recipients)
		{
			$error = error_no_recipients;
			return $error;
		}
		
		$recipients = explode(",", $this->survey->recipients);
		
		// in case there is just one
		if(count($recipients) == 0)
			$recipients = array($this->survey->recipients);
			
		$recipientList = array();
		
		foreach($recipients as $recipient)
		{
			if(!preg_match('/\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\.[A-Z]{2,4}\b/i', $recipient))
			{
				$error = error_invalid_recipients;
				array_push($recipientList, "<warning>$recipient</warning>");
			}
			else
				array_push($recipientList, $recipient);
		}
		
		if($error == error_invalid_recipients)
		{
			$this->survey->recipients = implode(",", $recipientList);
		}
		else
		{
			if(!$this->survey->message)
				$error = error_no_message;
		}
		return $error;
	}
}

?>