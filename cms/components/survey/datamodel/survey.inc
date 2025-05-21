<?php
/**\addtogroup survey
 * @{
 */


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
Fakoli::using("questionnaire");


class Survey extends AbstractQuestionnaire
{
	var $fields = array(
				"survey_id"							=>	'Number',
				"title"								=>	'String',
				"introduction"						=>	'Text',
				"start_date"						=>	'Date',
				"end_date"							=>	'Date',
				"instructions"						=>	'HTML',
				"user_id"							=>	'Number',
				"sender_email"						=>	'String',
				"recipients"						=>	'Text',
				"cc_recipients"						=>	'Text',
				"additional_recipients"				=>	'Text',
				"allow_anonymous_responses"			=>	'Boolean',
				"show_preview_before_submitting"	=>	'Boolean',
				"message"							=>	'Text',
				"confirmation_message"				=>	'HTML',
				"status"							=>	'String',
				"deleted"							=>	'Boolean'
	);

	var $relations = array(
				"Responses"				=>	'SurveyResponse',
				"Questions"				=>	'SurveyQuestion',
				"SurveyQuestionXrefs"	=>	'SurveyQuestionXref',
				"User"					=>	"",
				"Author"				=>	"",
				);

	var $fieldAliases = array(
			"cc_recipients"	=>	"CC Recipients",
			);

	static $statusTypes = array(
		"not sent"	=>	"Not Sent",
		"open"		=>	"Open",
		"closed"	=>	"Closed"
		);


	// Need to account for user tables that don't have "user_id"
	// as primary key.
	function User()
	{
		$mgr = new UserManager();
		$class = $mgr->getUserClass();
		$l_user = new $class();
		$pk = $l_user->getPrimaryKey();

		$authors = Query::create($class, "WHERE $pk =:user_id")
			->bind(":user_id", $this->user_id)
			->execute();
		return (count($authors) > 0) ? $authors[0] : null;
	}
	
	function Author()
	{
		return $this->User();
	}

	function SurveyQuestionXrefs($constraint = "ORDER BY sort_order")
	{
		return $this->getRelatedList('SurveyQuestionXref', "survey_id", $constraint);
	}

	function Questions($constraint = "ORDER BY sort_order")
	{
		if($this->survey_id)
			return $this->crossReference('SurveyQuestion', 'SurveyQuestionXref', $constraint);
	}

	function Responses($constraint = "")
	{
		return $this->getRelatedList('SurveyResponse');
	}

	/* for results aggregation */
	function Answers($constraint = "")
	{
		$query = "WHERE response_id IN (SELECT response_id FROM survey_response WHERE status = 'submitted' AND survey_id=:survey_id AND include_in_results=true)";
		if($constraint)
		{
			$query .= preg_replace("/WHERE/i", " AND", $constraint);
		}
		
		if($constraint)
		{
			$constraint = preg_replace("/WHERE/i", " AND", $constraint);
			$constraint =  preg_replace("/ question_id/", " survey_question_id", $constraint);
			$query .= $constraint;
		}
		
		trace("Email survey answers constraint ". $query, 3);
		return Query::create('SurveyAnswer', $query)
		->bind(":survey_id", $this->survey_id)
		->execute();
		
		$answers = query('SurveyAnswer', $query);
		return $answers;
	}

	function getTitle()
	{
		$title = $this->title;
		$title .= ($this->start_date > 0) ? " &ndash; " . $this->format("{start_date:F d, Y}") : "";

		return $title;
	}

	function isEditable()
	{
		if(!$this->survey_id)
			return true;
						
		return ($this->isAuthor() AND $this->status == "not sent") ? true : false;
	}

	function isSent()
	{
		return ($this->status == "not sent") ? false : true;
	}

	function isOpen()
	{
		return ($this->status == "open") ? true : false;
	}
	
	function setDefaults()
	{
		global $user;
		
		$this->status = "not sent";
		$this->user_id = $user->user_id;
	}

	/* set to closed if end_date is greater than today  */
	function isClosed()
	{
		if($this->status == "open" AND $this->end_date > 0)
		{
			$today = date("Y-m-d");
			if($this->end_date < $today)
			{
				$this->setStatus("closed");
			}
		}

		return ($this->status == "closed") ? true : false;
	}

	function isAuthor()
	{
		global $user;
		return (checkRole("admin,data") || $this->site_user_id == $user->site_user_id) ? true : false;

	}

	function setStatus($status)
	{
		$today = date("Y-m-d"); // today's date

		if($status == "open")
		{
			$this->filter = new InclusionFilter("start_date", "end_date", "status");
							
			if($this->start_date > 0)
			{
				if($this->end_date > 0) // reopening
					$this->end_date = 0;
			}
			else // opening for the first time
				$this->start_date = $today;

		}
		elseif($status == "closed")
		{
			$this->filter = new InclusionFilter("end_date", "status");
			$this->end_date = $today;
		}

		$this->status = $status;
		$this->save();
	}
	
	/**
	 * Called by SurveyResultsManager
	 * 
	 * @return Number
	 */
	function getRecipientCount()
	{
		$count = Query::create('SurveyResponse', "WHERE survey_id=:survey_id AND include_in_results=true")
			->bind(":survey_id", $this->survey_id)
			->executeValue("COUNT(1)");
			
		return $count;
	}
	
	/**
	 * Called by SurveyResultsManager
	 * 
	 * for survey aggregation
	 * 
	 * @return Number
	 */
	function getResponseCount()
	{
		$count = Query::create('SurveyResponse', "WHERE status = 'submitted' AND survey_id=:survey_id AND include_in_results=true")	
			->bind(":survey_id", $this->survey_id)
			->executeValue("COUNT(1)");
						
		return $count;
	}

	/**
	 * Called by SurveyResultsManager
	 * 
	 * @return array of SurveyResponse objects
	 */
	function getNonResponders()
	{
		$nonResponders = Query::create('SurveyResponse', "WHERE survey_id=:survey_id AND status != 'submitted' AND include_in_results=true")
			->bind(":survey_id", $this->survey_id)
			->execute();

		return $nonResponders;
	}
	
	function allowDelete()
	{
		if(!$this->survey_id)
			return true;

		return ($this->status != "open") ? true : false;
	}

	function __construct()
	{
		$this->table = "survey";
		$this->primary_key = "survey_id";
		$this->answer_class = SurveyAnswer;

		$this->pretty_class_name = "Survey";

		$this->DataItem(func_get_args());
	}
}



/**@}*/?>
