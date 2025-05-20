<?php
/**\addtogroup survey
 * @{
 */


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

Fakoli::using("questionnaire");

class SurveyResponse extends AbstractQuestionnaireResponse
{
	var $fields = array (
				"response_id"			=>	Number,
				"survey_id"				=>	Number,
				"token"					=>	String,
				"email"					=>	String,
				"last_modified"			=>	Date,
				"status"				=>	String,
				"include_in_results"	=>	Boolean,
	);
	
	var $fieldAliases = array(
			"include_in_results"	=>	"Include in Reslts",
	);

	var $relations = array (
				"Questions"				=>	Questions,
				"Answers"				=>	SurveyAnswer,
				"Survey"				=>	Survey,
	);

	var $statusTypes = array(
		"not_started"	=>	"Not Started",
		"in_progress"	=>	"In Progress",
		"submitted"		=>	"Submitted",
	);

	function Questions()
	{
		$survey = $this->Survey();

		if($survey)
			$questions = $survey->Questions();
			
		return $questions;
	}

	function Survey()
	{
		return $this->getRelated(Survey);
	}

	function Answers($constraint = "")
	{
		$answers = array();
		$questions = $this->Questions();
		foreach($questions as $question)
		{
			$answer = querySingle(SurveyAnswer, "WHERE response_id = {$this->response_id} AND survey_question_id = $question->survey_question_id");
			if($answer)
				array_push($answers, $answer);
		}

		return $answers;
	}

	function Questionnaire()
	{
		return $this->Survey()->Questionnaire();
	}

	function isSubmitted()
	{
		return ($this->status != "submitted") ? false : true;
	}

	function isEditable()
	{
		return ($this->status != "submitted") ? true : false;
	}


	function setStatus($status)
	{
		$this->last_modified = date('Y-m-d');
		$this->filter = new InclusionFilter("status", "last_modified");
		$this->status = $status;
		$rtn = $this->save();
	}
	
	static function findResponseByToken($survey_id, $token)
	{
		$responses = Query::create(SurveyResponse, "WHERE survey_id=:survey_id AND token=:token")
		->bind(":survey_id", $survey_id, ":token", $token)
		->execute();
			
		return (count($responses) > 0) ? $responses[0] : null;
	}
	
	
	function setDefaults()
	{
		$this->status = "not_started";
		$this->include_in_results = true;
	}

	/*
	 * merge_code
	 * 
	 * Allow a user to get to a survey landing page with just the survey_id
	 * and they can enter the token from there.
	 * 
	 * Called by merge code and response tester block
	 */
	function getEditUrl()
	{
		$mgr = new SurveyResponseManager($this);		
		return $mgr->getEditUrl($this->Survey());
	}

	/**
	 * 	Called by merge code and response tester block
	 * 
	 * @return string
	 */
	function getIntroUrl()
	{
		$mgr = new SurveyResponseManager($this);		
		return $mgr->getIntroUrl($this->Survey());
	}
	
	function isTester()
	{
		return ($this->include_in_results) ? false : true;
	}
	
	function excludeResponse($include_in_results = false)
	{
		$this->include_in_results = false;
		$this->filter = new InclusionFilter("include_in_results");
		$this->save();
	}

	
	/**
	 * reinitialize token for a test user.
	 */
	function reset()
	{
		if(!$this->response_id) return;
		if(!$this->isTester()) return;
	
		$this->setStatus("not_started");
		$answer = new SurveyAnswer();
		$answer->delete("WHERE response_id={$this->response_id}");
	}
	

	function __construct()
	{
		$this->table = "survey_response";
		$this->primary_key = "response_id";
		$this->answer_class = SurveyAnswer;

		$this->DataItem(func_get_args());
	}
}



/**@}*/?>
