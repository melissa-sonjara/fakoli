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

Fakoli::using("questionnaire");
Fakoli::usingFile("/cms/components/questionnaire/questionnaire_manager.inc");
Fakoli::usingFile("/cms/components/questionnaire/results_graphs.inc");

class SurveyManager
{
	function __construct()
	{
		
	}


	static function surveyTabs($key = "")
	{
		$tabs = SurveyManager::getSurveyTabs($key);
		$qs = ($key) ? "survey_id=$key" : "";
		$tabBar = new TwoLevelTabBar($tabs, $qs);

		return $tabBar;
	}

	static function getSurveyTabs($key)
	{
		$formTitle = "Create a Survey";	
		$viewTitle = "Preview/Send";
		
		if($key)
		{
			$survey = new Survey($key);
			$formTitle = ($survey->isEditable()) ? "Edit Survey" : "Survey Details";
			$viewTitle = (!$survey->isSent()) ? "Preview/Send" : "Manage Survey";
		}
	
		$images[0] = "<img src='/fakoli/images/tab_create_survey.gif' style='display: inline-block;vertical-align: middle; border: none'/>&nbsp;";
		$images[1] = "<img src='/fakoli/images/tab_view_results.gif' style='display: inline-block;vertical-align: middle; border: none'/>&nbsp;";

		$tabs = array(
		$images[0] . $formTitle	=> array(
				"Introduction"			=>	"survey_form",
				"Questions"				=>	"survey_questions",
				"Email"					=>	"survey_email",
				"Confirmation"			=>	"survey_confirmation",
				$viewTitle				=>	"survey_preview"
				),
		
				$images[1] . "View Results"	=> array(
				"Survey Responses"		=>	"survey_results",
				"Graphical Data"		=>	"survey_data",
				"Tabular View"			=>	"survey_data_table",
				"Spreadsheet View"		=>	"survey_spreadsheet"
				),
			);
					
			return $tabs;
	}

} // end SurveyManager


class SurveySendManager extends AbstractQuestionnaireSendManager
{
	function __construct($survey)
	{
		parent::__construct($survey);
	}
	
	function getComponentName()
	{
		return "survey";
	}
	
	function getResponseClass()
	{
		return SurveyResponse;
	}
	
	function getResultsManager()
	{
		return new SurveyResultsManager($this->item);
	}
	
	function getReminderDialog()
	{
		return "survey_reminder_form";
	}
		
	function getCreateManager()
	{
		return new SurveyCreateManager($this->item);
	}
	
} // end SurveySendManager

/*
 * Description: provides the more generic questionnaire
 * classes with the specifics of the survey datamodels and
 * page identifiers. For example, lets the QuestionSet
 * class know the identifier name of the survey question form.
 *
 * @author Janice Gallant for Sonjara, Inc.
 *
 * 10/13/2010
 */
class SurveyCreateManager extends AbstractQuestionnaireCreateManager
{
	/**
	 * Creates a new SurveyCreateManager object.
	 */
	function __construct($survey)
	{
		$this->item = $survey;
	}
	
	function getQuestionClass()
	{
		return SurveyQuestion;
	}
	
	function getComponentName()
	{
		return "survey";
	}
	
	function getXrefClass()
	{
		return SurveyQuestionXref;
	}
	
	/*
	 * Called by QuestionnaireForm
	 */
	function getQuestionKey()
	{
		return "survey_question_id";
	}

	function getAnswerKey()
	{
		return "survey_answer_id";
	}

	/*
	 * Called by QuestionRenderer
	 */
	function getQuestionNumber($question)
	{
		return QuestionnaireHelper::getQuestionNumber($this->item, $this->item->Questions(), $question);
	}

	/*
	 * Called by QuestionnaireForm
	 * 
	 * If question_number pseudo field is set here then it won't need to be
	 * reset by each instance of QuestionRenderer
	 */
	function getQuestions()
	{
		$questions = $this->item->Questions("ORDER BY sort_order");
		return QuestionnaireHelper::setQuestionNumbers($this->item, $questions);
	}

	function isEditable()
	{
		return $this->item->isEditable();
	}

	function getPrimaryKey()
	{
		return $this->item->getPrimaryKey();
	}

	// The class that stores the answers
	function getAnswerClass()
	{
		return "SurveyAnswer";
	}

	function getAnswers()
	{
		return null;
	}
	
	function logicalDeleteQuestionnaire()
	{
		$item = $this->item;
		
		if($item->isOpen())
		{
			$item->setStatus("closed");
		}
		parent::logicalDeleteQuestionnaire();
	}
	
	function getCloneFields()
	{
		return array("introduction", "instructions", "sender_email", "user_id", "message", "confirmation_message",
				"allow_anonymous_responses", "show_preview_before_submitting");
	}

} // end class SurveyManager


class SurveyDashboardManager extends AbstractQuestionnaireDashboardManager
{
	function __construct($survey)
	{
		$this->item = $survey;
	}
	
	function getComponentName()
	{
		return "survey";
	}
	
	function getCreateManager($item)
	{
		return new SurveyCreateManager($item);
	}
	
	function getResultsManager($item)
	{
		return new SurveyResultsManager($item);
	}
	
	function getSendManager($item)
	{
		return new SurveySendManager($item);
	}
	
	function getCloneDialog()
	{
		return "clone_survey";
	}
	
	function getQuestionnaireDeleteHandler()
	{
		return "delete_survey";
	}
		
	/**
	 * Override default of "survey_responses"
	 */
	function getResultsPageIdentifier()
	{
		return "survey_results";
	}
}

class SurveyResponseManager extends AbstractQuestionnaireResponseManager
{
	function __construct($response)
	{
		$this->item = $response;
	}
	
	function getComponentName()
	{
		return "survey";
	}
	
	function getQuestionClass()
	{
		return SurveyQuestion;
	}

	function getXrefClass()
	{
		return SurveyQuestionXref;
	}
	
	/**
	 * For response intro page, need to create a 
	 * default instance of response manager.
	 * 
	 * @param Number $survey_id
	 * @return SurveyResponseManager
	 */
	static function create($survey_id)
	{
		// create for js manager
		$response = new SurveyResponse();
		$response->response_id = 0;
		$response->status = "not_started";
		$response->survey_id = $survey_id;
		
		return new SurveyResponseManager($response);
	}
	
	/*
	 * Called by QuestionnaireForm
	 */
	function getQuestionKey()
	{
		return "survey_question_id";
	}
	

	function getAnswerKey()
	{
		return "survey_answer_id";
	}
	

	// The class that stores the answers
	function getAnswerClass()
	{
		return "SurveyAnswer";
	}

	function getQuestions()
	{
		$questions = $this->item->Questions("ORDER BY sort_order");
		return QuestionnaireHelper::setQuestionNumbers($this->item, $questions);
	}

	/**
	 * When SurveyCreateManager is used to draw the Survey, we are looking
	 * at a preview so no need to retrieve answers.
	 */
	function getAnswers()
	{
		return $this->item->Answers();
	}


	// How respondent is described in spreadsheet view.
	// For some surveys, it will by by full name.
	function getRespondent()
	{
		return $this->item->token;
	}
	
	/*
	 * Called by QuestionRenderer
	 */
	function getQuestionNumber($question)
	{
		return QuestionnaireHelper::getQuestionNumber($this->item, $this->item->Questions(), $question);
	}		
}


/*
 * Handle getting total results/response counts for one survey for
 * display of results.
 */
class SurveyResultsManager extends AbstractQuestionnaireResultsManager
{
	function __construct($survey)
	{
		$this->item = $survey;
	}
	
	function getComponentName()
	{
		return "survey";
	}
		
	function getQuestionClass()
	{
		return SurveyQuestion;
	}

	function getXrefClass()
	{
		return SurveyQuestionXref;
	}

	function getResponseClass()
	{
		return SurveyResponse;
	}
	
	/*
	 * Called by QuestionnaireForm
	 */
	function getQuestionKey()
	{
		return "survey_question_id";
	}
	
	function getQuestions()
	{
		$questions = $this->item->Questions("ORDER BY sort_order");
		return QuestionnaireHelper::setQuestionNumbers($this->item, $questions);
	}
	
	function Responses($constraint = "")
	{
		return $this->item->Responses($constraint);
	}

	function getResponseManager($response)
	{
		return new SurveyResponseManager($response);
	}

	/*
	 * Only get answers to submitted (not in progress) questionnaires/surveys
	 */
	function getAnswers()
	{
		return $this->item->Answers();
	}


	function getRespondentColumnHeading()
	{
		return "Token";
	}

	function getResponseViewDialog()
	{
		return "survey_response_view";
	}
	
	/*
	 * Called by QuestionRenderer
	 */
	function getQuestionNumber($question)
	{
		return QuestionnaireHelper::getQuestionNumber($this->item, $this->item->Questions(), $question);
	}

	/* for survey aggregation */
	function getResponseCount()
	{
		return $this->item->getResponseCount();
	}

	function getRecipientCount()
	{
		return $this->item->getRecipientCount();
	}
	
	function getSendManager()
	{
		return new SurveySendManager($this->item);
	}	
} // end AbstractQuestionnaireResultsManager

class SurveyAggregatedResultsManager //extends AbstractQuestionnaireAggregatedResultsManager
{
	function __construct($item)
	{
		$this->item = $item;
	}
	
	function getQuestionClass()
	{
		return "SurveyQuestion";
	}
	
	function getXrefClass()
	{
		return SurveyQuestionXref;
	}	
	
	function Responses()
	{
		return Query::create('SurveyResponse', "WHERE status='submitted'")->execute();
	}
	
	function getAnswers()
	{
		$constraint = "WHERE response_id IN (SELECT response_id FROM survey_response WHERE status = 'submitted')";
	
		$answers = Query::create('SurveyAnswer', $constraint)->execute();
		return $answers;	
	}
}

/**@}*/?>
