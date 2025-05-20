<?php
/**\addtogroup questionnaire
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

abstract class AbstractQuestionnaireResponse extends DataItem
{
	function AbstractQuestionnaireResponse()
	{
		
	}
	
	abstract function Questions();

	/**
	 * If key field is not null, retrieve the answers linked to this
	 * respondent.
	 */
	abstract function Answers();
	
	abstract function isSubmitted();

	abstract function isEditable();
	
	abstract function setDefaults();
	
	/**
	 * Set the response status and save
	 */
	abstract function setStatus($status);
	
	
}

class QuestionnaireResponse extends AbstractQuestionnaireResponse
{
	var $fields = array (
				"response_id"		=>	Number,
				"questionnaire_id"	=>	Number,
				"user_id"			=>	Number,
				"date_submitted"	=>	DateTime,
				"status"			=>	String,
	);

	var $relations = array (
				"Questions"				=>	Questions,
				"Answers"				=>	QuestionnaireAnswer,
				"Questionnaire"			=>	Questionnaire,
	);

	var $statusTypes = array(
		"not_started"	=>	"Not Started",
		"in_progress"	=>	"In Progress",
		"submitted"		=>	"Submitted",
	);

	
	function Questions()
	{
		$questionnaire = $this->Questionnaire();

		if($questionnaire)
			$questions = $questionnaire->Questions();
			
		return $questions;
	}

	function Questionnaire()
	{
		return $this->getRelated(Questionnaire);
	}
	
	function Answers($constraint = "")
	{
		if(!$this->response_id)
		{
			return null;
		}
		return $this->getRelatedList(QuestionnaireAnswer, "response_id", $constraint);
	}
	
	function isSubmitted()
	{
		return ($this->status != "submitted") ? false : true;
	}
	
	function setDefaults()
	{
		$this->status = "not_started";
	}

	function isEditable()
	{
		return ($this->status != "submitted") ? true : false;
	}

	function setStatus($status)
	{
		$this->date_submitted = date('Y-m-d');
		$this->status = $status;
		$this->filter = new InclusionFilter("status", "date_submitted");
		$this->save();
	}

	function __construct()
	{
		$this->table = "questionnaire_response";
		$this->primary_key = "response_id";
	
		parent::__construct(func_get_args());
		
		$this->default_format = "{Respondent.first_name} {Respondent.last_name}";
	}
}



/*
 * Dummy dataitem class to enable use of dataview on
 * option results
 */
class OptionAnswer extends DataItem
{
	var $fields = array(
					"question_id"		=>	Number,
					"option_number"		=>	Number, // position in list
					"option"			=>	String,
					"count"				=>	Number
	);

	function __construct($question_id, $option_number, $option, $count)
	{
		$this->table = "option_answer";
		$this->primary_key = "question_id";
		$this->question_id = $question_id;
		$this->option_number = $option_number;
		$this->option = $option;
		$this->count = $count;
	}
}


/**
 * Dummy datamodel for building SpreadsheetView
 *
 */
class SpreadsheetResponse extends DataItem
{
	var $fields = array(
			"name"		=>	String,
			//question/answer fields built at run time
	);

	function __construct()
	{
		$this->table = "spreadsheet_response";
		
		// pk value does not need to match questionnaire item
		$this->primary_key = "questionnaire_id";

		parent::__construct(func_get_args());
	}
}
/**@}*/?>
