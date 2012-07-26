<?php
/**************************************************************

Copyright (c) 2007,2008 Sonjara, Inc

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

Except as contained in this notice, the name(s) of the above
copyright holders shall not be used in advertising or otherwise
to promote the sale, use or other dealings in this Software
without prior written authorization.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

*****************************************************************/

Fakoli::usingFeature("auto_form");

class ValidateSurvey extends ValidateTabSet
{
	var $survey;

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
		if(!$this->validateQuestionnaire())
			redirect("/survey_questions?survey_id={$this->survey->survey_id}&error=1");

		if(!$this->validateEmail())
			redirect("/survey_email?survey_id={$this->survey->survey_id}&error=1");

		return true;
	}

	function validateQuestionnaire()
	{
		$questions = $this->survey->Questions();
		if(count($questions) == 0)
		{
			$this->msg = "You must select or create the survey questions.";
			return false;
		}

		return true;
	}

	function validateEmail()
	{
		if(!$this->survey->recipients)
			$this->msg = "You must enter the survey recipient email addresses.";
		else
		{
			$recipients = explode(",", $this->survey->recipients);

			// in case there is just one
			if(count($recipients) == 0)
				$recipients = array($this->survey->recipients);
				
			foreach($recipients as $recipient)
			{
				if(!preg_match('/\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\.[A-Z]{2,4}\b/i', $recipient))
				{
					if($this->msg) $this->msg .= "<br>";
					$this->msg .= "<warning>$recipient</warning> is an invalid email address.<br>";
				}
			}
		}

		if(!$this->survey->message)
		{
			if($this->msg) $this->msg .= "<br>";
			$this->msg .= "You must select or create an email message.";
		}

		if($this->msg)
			return false;
			
		return true;
	}
}



class ValidateSurveyResponse extends ValidateTabSet
{
	var $response;
	var $error = 0;
	var $msg;

	function ValidateSurveyResponse($response)
	{
		$this->response = $response;
	}

	function Validate()
	{
		$questionnaire = new QuestionnaireForm(new SurveyResponseManager($this->response), true);
		$this->msg = $questionnaire->validate();

		if($msg != "")
			redirect("/survey_response_form?response_id={$this->response->response_id}&error=1");

		return true;
	}

	function ValidateView()
	{
		$questionnaire = new QuestionnaireForm(new SurveyResponseManager($this->response), true);
		$this->msg = $questionnaire->validate();

		if($this->msg != "")
			return false;
			
		return true;
	}
}


?>