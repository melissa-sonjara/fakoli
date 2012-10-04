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


class SurveyResponse extends DataItem
{
	var $fields = array (
				"response_id"		=>	Number,
				"survey_id"			=>	Number,
				"token"				=>	String,
				"email"				=>	String,
				"last_modified"		=>	Date,
				"status"			=>	String,
	);

	var $relations = array (
				"Questions"				=>	Questions,
				"Answers"				=>	SurveyAnswer,
				"Survey"				=>	Survey,
				"MergeCodes"			=>	MergeCode,
	);

	var $statusTypes = array(
		"not_started"	=>	"Not Started",
		"in_progress"	=>	"In Progress",
		"submitted"		=>	"Submitted",
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
				$surveyResponse->status = "not_started";
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

	function MergeCodes()
	{
		return Query::create(MergeCode, "WHERE class_name = :class_name")
			->bind(":class_name", get_class($this))
			->execute();
	}


	/* merge code */
	function getIntroUrl()
	{
		global $config;

		// need full url for links placed inside email messages
		$link = "http://". $config["http_host"] . "/survey_response_intro?survey_id={$this->survey_id}";
			
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



?>