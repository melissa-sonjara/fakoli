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

require_once realpath(dirname(__FILE__)."/../../framework/data_item.inc");
require_once  $config['homedir']."/datamodel/program.inc";



define("survey_not_sent", 0);
define("survey_open", 1);
define("survey_closed", 2);

define("response_not_started", 0);
define("response_in_progress", 1);
define("response_submitted", 2);

class EmailSurvey extends DataItem
{
	var $fields = array(
				"survey_id"			=>	Number,
				"title"				=>	String,
				"introduction"		=>	HTML,
				"template_id"		=>	Number,
				"questionnaire_id"	=>	Number,
				"program_id"		=>	Number,
				"recipients"		=>	Text,
				"email_from"		=>	String,
				"message"			=>	Text,
				"start_date"		=>	Date,
				"end_date"			=>	Date,
				"site_user_id"		=>	Number,
				"status"			=>	Number,
				"deleted"			=>	Boolean
	);
	
	var $relations = array(
				"EmailTemplate"		=>	EmailTemplate,
				"Questionnaire"		=>	Questionnaire,
				"Program"			=>	Program,
				"Responses"			=>	SurveyResponse
	);
	
	
	var $statusTypes = array(
				0	=>	"Not Sent",
				1	=>	"Open",
				2	=>	"Closed"
	);
	
	function getRequiredText($index, $error = false)
	{	
		static $text = array (
				1	=>  "One or more email addresses are invalid.",
				2	=>	"You must select the survey questions.",
				3	=>	"You must select an email template."	
				);
	
		$html = ($error) ? "<p><warning>{$text[$index]}</warning></p>"
				: "<p><b>$text[$index]</b></p>";
				
		return $html;
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
	
	function EmailTemplate()
	{
		// getRelated not working here
		if($this->template_id)
			return querySingle(EmailTemplate, "WHERE template_id={$this->template_id}");
	}
	
	function Questionnaire()
	{
		// getRelated not working here
		if($this->questionnaire_id)
			return querySingle(Questionnaire, "WHERE questionnaire_id={$this->questionnaire_id}");
	}
	
	function Program()
	{
		$program = ($this->program_id) ? $this->getRelated(Program) : Program::getDefaultProgram();
		
		return $program;
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
		return "survey_view?survey_id={$this->survey_id}";
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
	
	function EmailSurvey()
	{
		$this->table = "email_survey";
		$this->primary_key = "survey_id";
		$this->answer_class = SurveyAnswer;
		
		$this->pretty_class_name = "Survey";
		
		$this->DataItem(func_get_args());
	}
}


class EmailTemplate extends DataItem
{
	var $fields = array(
				"template_id"			=>	Number,
				"name"					=>	String,
				"subject"				=>	String,
				"message"				=>	HTML,
				"message_footer"		=>	HTML,
				"deleted"				=>	Boolean
	);
	
	function allowDelete()
	{
		if(!$this->template_id)
			return true;
			
		$count = queryValue(EmailSurvey, "COUNT(1)", "WHERE template_id={$this->template_id}");
		
		return ($count > 0) ? false : true;
	}
	
	
	static function getEmailTemplateList()
	{
		global $user;
		
		$templateList = array();
		$templateList[0] = "";
		
		$eTemplates = query(EmailTemplate, "ORDER BY name");
		if(count($eTemplates) > 0)
		{
			foreach($eTemplates as $eTemplate)
				$templateList[$eTemplate->template_id] = $eTemplate->name;
		}
		
		return $templateList;
	}
	
	function EmailTemplate()
	{
		$this->table = "email_template";
		$this->primary_key = "template_id";
		
		$this->DataItem(func_get_args());
	}
}



class EmailCodeMerge extends DataItem
{
	var $fields = array(
			"merge_id"		=>	Number,
			"name"			=>	String,
			"description"	=>	Text,
			"map"			=>	String,
			"function"		=>	Boolean,
			"class_name"	=>	String,
			"required"		=>	Boolean
	);

	
	function EmailCodeMerge()
	{
		$this->table = "email_code_merge";
		$this->primary_key = "merge_id";
		
		$this->DataItem(func_get_args());		
	}
}



class SurveyResponse extends DataItem
{
	var $fields = array (
				"response_id"		=>	Number,
				"survey_id"			=>	Number,
				"token"				=>	String,
				"email"				=>	String,
				"last_modified"		=>	Date,
				"status"			=>	Number
	);
	
	var $relations = array (
				"Questions"		=>	Questions,
				"Answers"		=>	SurveyAnswer,
				"EmailTemplate"	=>	EmailTemplate,
				"Questionnaire"	=>	Questionnaire,
				"EmailSurvey"	=>	EmailSurvey,
				"EmailCodeMerges"	=>	EmailCodeMerge,
				"Program"			=>	Program
	);

	var $statusTypes = array(
				0	=>	"Not Started",
				1	=>	"In Progress",
				2	=>	"Submitted"
	);
	
	function getRequiredText($index, $error = false)
	{	
		static $text = array (1	=>	"You must answer all required questions.",
				);
	
		$html = ($error) ? "<p><warning>{$text[$index]}</warning></p>"
				: "<p><b>$text[$index]</b></p>";
				
		return $html;
	}
	
	
	function Questions()
	{
		$questionnaire = $this->Questionnaire();
		
		if($questionnaire)
			$questions = $questionnaire->Questions("ORDER BY question_number");
			
		return $questions;
	}

	function EmailSurvey()
	{
		return $this->getRelated(EmailSurvey);
	}
	
	/* for email template code merge */
	function Program()
	{
		// Don't use cross reference b/c need EmailSurvey to handle case where
		// no program_id
		$program = $this->EmailSurvey()->Program();
			
		return $program;
	}
	
	function Answers($constraint = "")
	{
		$answers = array();
		$questions = $this->Questions();
		foreach($questions as $question)
		{
			$answer = querySingle(SurveyAnswer, "WHERE response_id = {$this->response_id} AND question_id = $question->question_id");
			if($answer)
				array_push($answers, $answer);
		}

		return $answers;
	}
	
	function EmailTemplate()
	{
		return $this->crossReference(EmailTemplate, EmailSurvey);
	}
	
	function Questionnaire()
	{
		return $this->EmailSurvey()->Questionnaire();
	}
	
	function getQuestionnaireId()
	{
		return $this->Questionnaire()->questionnaire_id;
	}
	
	function isSubmitted()
	{
		return ($this->status < 2) ? false : true;
	}
	
	function isEditable()
	{
		return ($this->status != response_submitted) ? true : false;
	}
	
	/*
	 * generateToken
	 * 
	 * Create a token to grant access to enter responses to
	 * the questionnaire/survey. Before accepting a generated
	 * token, we need to make sure that that token does not
	 * exist already.
	 */
	static function generateToken($survey_id, $email)
	{
		if(!$surveyResponse)
		{
			$surveyResponse = new SurveyResponse();
		
			do
			{
				$code = "";
				
				for($i = 0; $i < 5; ++$i)
				{
					$code .= chr(rand(ord('A'), ord('Z')));
				}
			
				$surveyResponse->token = $code;
				$surveyResponse->survey_id = $survey_id;
				$surveyResponse->email = $email;
			}
			while(!$surveyResponse->insert());
		}
		
		return $surveyResponse;
	}
	
	function setStatus($status)
	{
		$this->last_modified = date('Y-m-d');
		$this->filter = new InclusionFilter("status", "last_modified");
		$this->status = $status;
		$rtn = $this->save();
	}	
	
	function EmailCodeMerges()
	{
		return query(EmailCodeMerge, "WHERE class_name = '". get_class($this). "'");
	}
	
	/* 
	 * Allow a user to get to a survey landing page with just the survey_id
	 * and they can enter the token from there.
	 */
	function getEditUrl()
	{
		global $config;

		// need full url for links placed inside email messages
		$link = "http://". $config["http_host"] . "/survey_response_form?survey_id={$this->survey_id}";
		if($this->token)
			$link .= "&token={$this->token}";
			
		return $link;
	}
	
	/* merge code */
	function getIntroUrl()
	{
		global $config;

		// need full url for links placed inside email messages
		$link = "http://". $config["http_host"] . "/survey_intro?survey_id={$this->survey_id}";
			
		return $link;
	}
	
	function SurveyResponse()
	{
		$this->table = "survey_response";
		$this->primary_key = "response_id";
		$this->answer_class = SurveyAnswer;
		
		$this->DataItem(func_get_args());
	}
}


class SurveyAnswer extends DataItem
{
	var $fields = array (	"answer_id"				=>	Number,
							"response_id"			=>	Number, 
							"question_id"			=>	Number,
							"value"					=>	String
						);
						
	var $relations = array ("Question"	=>	Question
							);

	function Question()
	{
		return $this->getRelated(Question, "question_id");
	}
	
	function SurveyAnswer()
	{
		$this->table = "survey_answer";
		$this->primary_key = "answer_id";
		
		$this->DataItem(func_get_args());
	}
}