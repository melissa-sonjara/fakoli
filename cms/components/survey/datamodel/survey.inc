<?php
/**************************************************************

 Copyright (c) 2010 Sonjara, Inc

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


/*
 * Title: survey.inc
 * 
 * Description: Datamodel for Email Templates.
 * 
 * author: Janice Gallant for Sonjara, Inc.
 * 
 * date: 2/18/10
 * 
 */


define("survey_not_sent", 0);
define("survey_open", 1);
define("survey_closed", 2);

define("response_not_started", 0);
define("response_in_progress", 1);
define("response_submitted", 2);

define("error_no_recipients", 1);
define("error_invalid_recipients", 2);
define("error_no_questions", 3);
define("error_no_message", 4);


class Survey extends DataItem
{
	var $fields = array(
				"survey_id"					=>	Number,
				"title"						=>	String,
				"introduction"				=>	Text,
				"start_date"				=>	Date,
				"end_date"					=>	Date,
				"user_id"					=>	Number,
				"sender_email"				=>	String,
				"recipients"				=>	Text,
				"message"					=>	Text,
				"status"					=>	Number,
				"deleted"					=>	Boolean
	);
	
	var $relations = array(
				"Responses"				=>	SurveyResponse,
				"Questions"				=>	SurveyQuestion,
				"SurveyQuestionXrefs"	=>	SurveyQuestionXref,
				"User"					=>	""
	);
	
	
	var $statusTypes = array(
				0	=>	"Not Sent",
				1	=>	"Open",
				2	=>	"Closed"
	);
	
	function getRequiredText($index, $error = false)
	{	
		static $text = array (
				1	=>	"You must enter the survey recipient email addresses.",
				2	=>  "One or more email addresses are invalid.",
				3	=>	"You must select or create the survey questions.",
				4	=>	"You must select or create an email message."
				);
	
		$html = ($error) ? "<p><warning>{$text[$index]}</warning></p>"
				: "<p><b>$text[$index]</b></p>";
				
		return $html;
	}
	
	function User()
	{
		$mgr = new UserManager();
		return $this->getRelated($mgr->getUserClass());
	}
	
	function SurveyQuestionXrefs($constraint = "ORDER BY sort_order")
	{
		return $this->getRelatedList(SurveyQuestionXref, "survey_id", $constraint);
	}
	
	function Questions($constraint = "")
	{
		if($this->survey_id)
			return $this->crossReference(SurveyQuestion, SurveyQuestionXref, $constraint);
	}
	
	
	function getStatusInfoText()
	{
		switch ($this->status) {
			case survey_not_sent:
				$text = "This survey has not yet been sent.";
				break;
			
			case survey_open:
				$text = "This survey was opened on ". formatDate($this->start_date) .".";
				break;
				
			case survey_closed:
				$text = "This survey was opened on ". formatDate($this->start_date) ." and closed on ". formatDate($this->end_date) .".";	
				break;	
		}

		return $text;
	}
	
	function getInfoMessageText($index = 0)
	{
		if($index == 2) $error = true;
			
		static $text = array (
				1	=>  "Reminder emails have been sent.",
				2	=>	"Reminder emails could not be sent."
				);
	
		$msg = $text[$index];
		if($msg)
		{
			$html = ($error) ? "<p><warning>{$msg}</warning></p>"
				: "<p><b>$msg</b></p>";
		}		
		return $html;
	}
	
	function SurveyQuestionXref($constraint = "")
	{
		return $this->getRelatedList(SurveyQuestionXref, "");
	}
	
	function getStatusCategories()
	{
		return array_flip($this->statusTypes);
	}
	
	function Responses($constraint = "")
	{
		return $this->getRelatedList(SurveyResponse, "", $constraint);
	}
	
	/* for results aggregation */
	function Answers($constraint = "")
	{
		if ($constraint)
		{
			$query = "$constraint AND response_id IN (SELECT response_id FROM survey_response WHERE status = "
			. response_submitted. " AND survey_id={$this->survey_id})";
		}
		else
		{
			$query = "WHERE response_id IN (SELECT response_id FROM survey_response WHERE status = "
			. response_submitted. " AND survey_id={$this->survey_id})";
		}
		
		$answers = query(SurveyAnswer, $query);
		return $answers;
	}
	
	function getTitle()
	{
		$title = $this->title;
		$title .= ($this->start_date > 0) ? " &ndash; " . formatDate($this->start_date) : "";
		
		return $title;
	}
	
	function getEditUrl()
	{
		return "survey_form?survey_id={$this->survey_id}";
	}
	
	function getViewUrl()
	{
		return "survey_manage?survey_id={$this->survey_id}";
	}
	
	function isEditable()
	{
		if(!$this->survey_id)
			return true;
			
		global $user;
		return ($this->isAuthor() AND $this->status == survey_not_sent) ? true : false;
	}
	
	function isOpen()
	{	
		return ($this->status == survey_open) ? true : false;
	}
	
	/* set to closed if end_date is greater than today  */
	function isClosed()
	{
		if($this->status == survey_open AND $this->end_date > 0)
		{
			$today = date("Y-m-d");
			if($this->end_date < $today)
			{
				$this->setStatus(survey_closed);
			}
		}
		
		return ($this->status == survey_closed) ? true : false;
	}
	
	function isAuthor()
	{
		global $user;
		return (checkRole("admin,data") || $this->site_user_id == $user->site_user_id) ? true : false;
		
	}
	
	function setStatus($status)
	{
		$today = date("Y-m-d"); // today's date

		if($status == survey_open)
		{
			$this->filter = new InclusionFilter("start_date", "end_date", "status", "program_id");
			
			if($this->start_date > 0)
			{
				if($this->end_date > 0) // reopening
					$this->end_date = 0;
			}
			else // opening for the first time
				$this->start_date = $today;
				
		}
		elseif($status == survey_closed)	
		{
			$this->filter = new InclusionFilter("end_date", "status", "program_id");
			$this->end_date = $today;
		}

		$this->status = $status;
		$this->save();
	}
	
	/* for survey aggregation */
	function getResponseCount()
	{
		$count = queryValue(SurveyResponse, "COUNT(1)", "WHERE status = " . response_submitted . " AND survey_id = {$this->survey_id}");
		return $count;
	}
	
	function getRecipientCount()
	{
		$count = queryValue(SurveyResponse, "COUNT(1)", "WHERE survey_id = {$this->survey_id}");
		return $count;
		
	}
	
	static function getSenderEmail()
	{
		$email = Settings::getValue("survey", "sender_email");
		if(!$email)
			$email = Settings::getValue("settings", "email_contact");
		if(!$email)
			$email = Settings::getValue("settings", "email_from");

		return $email;
	}
	
	function Survey()
	{
		$this->table = "survey";
		$this->primary_key = "survey_id";
		$this->answer_class = SurveyAnswer;
		
		$this->pretty_class_name = "Survey";
		
		$this->DataItem(func_get_args());
	}
}


