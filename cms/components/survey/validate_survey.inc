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