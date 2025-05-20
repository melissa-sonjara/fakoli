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


class SurveyQuestion extends AbstractQuestion
{
	var $fields = array (	"survey_question_id"	=>	Number,
							"question_type_id"		=>	Number,
							"question"				=>	HTML,
							"question_name"			=>	String,
							"options"				=>	Text,
							"required"				=>	Number,
							"char_limit"			=>	Number,
							"num_rows"				=>	Number,
							"context_help"			=>	Text
	);

	var $relations = array (
					"Answer"				=>	Answer,
					"Survey"				=>	Survey,
					"SurveyQuestionXref"	=>	SurveyQuestionXref,
					"QuestionType"			=>	QuestionType
	);

	// Answer to one object's survey question
	function Answer($response_id)
	{
		$answer = Query::create(SurveyAnswer, "WHERE response_id=:response_id AND survey_question_id=:survey_question_id")
			->bind(":response_id", $response_id, ":survey_question_id", $this->survey_question_id)
			->executeSingle();

		return $answer;
	}

	function Survey()
	{
		return $this->crossReference(Survey, SurveyQuestionXref);
	}

	function QuestionType()
	{
		return $this->getRelated(QuestionType);
	}
		
	function SurveyQuestionXref($survey_id)
	{
		if($this->survey_question_id)
			return querySingle(SurveyQuestionXref, "WHERE survey_question_id = {$this->survey_question_id} AND survey_id = $survey_id");
	}

	function isEditable()
	{
		if(!$this->survey_question_id)
			return true;

		$count = queryValue(SurveyAnswer, "COUNT(1)", "WHERE survey_question_id={$this->survey_question_id}");

		return ($count > 0) ? false : true;
	}


	/*
	 * If the question is editable and is linked only to this survey or
	 * survey template, then allow delete. This function is called
	 * from survey_question_form to determine if a user can delete his/her
	 * own question.
	 */
	function allowDelete()
	{
		if(!$this->survey_question_id)
			return true;
				
		if(!$this->isEditable())
			return false;
				
		$count = queryValue(SurveyQuestionXref, "COUNT(1)", "WHERE survey_question_id={$this->survey_question_id}");

		return ($count > 1) ? false : true;
	}

	function __construct()
	{
		$this->table = "survey_question";
		$this->primary_key = "survey_question_id";

		$this->DataItem(func_get_args());
	}
}


/**@}*/?>
