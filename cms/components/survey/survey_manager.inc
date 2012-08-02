<?
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

class SurveyManager
{
	function SurveyManager()
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
		$previewTitle = "Preview/Send";
		
		if($key)
		{
			$survey = new Survey($key);
			$formTitle = ($survey->isEditable()) ? "Edit Survey" : "Survey Details";
			if($survey->status != "not sent") $previewTitle = "View";
		}

		$images[0] = "<img src='/fakoli/images/tab_create_survey.gif' style='display: inline-block;vertical-align: middle; border: none'/>&nbsp;";
		$images[1] = "<img src='/fakoli/images/tab_view_results.gif' style='display: inline-block;vertical-align: middle; border: none'/>&nbsp;";

		$tabs = array(
		$images[0] . $formTitle	=> array(
				"Introduction"			=>	"survey_form",
				"Questions"				=>	"survey_questions",
				"Email"					=>	"survey_email",
				$previewTitle			=>	"survey_preview"
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
	
	static function getSurveyProgressBar($survey)
	{
		$percentage = 0;
		$total = $survey->getRecipientCount();
		if($total > 0)
		{
			$responseCount = $survey->getResponseCount();
			$percentage = round($responseCount / $total * 100, 0);
		}

		$html = "<div class='col2'>\n";
		$html .= $percentage . "%";
		if ($responseCount) $html .= " <span style='font-size: 8pt'>($responseCount&nbsp;of&nbsp;$total)</span>";
		$html .= "</div>\n";
		$html .= "<div class='progress_bar'>\n";
		$html .= "<div class='progress' style='width: $percentage%'>\n";
		$html .= "</div></div>\n";

		return $html;
	}


	/**
	 * Retrieve all the question set for all surveys but
	 * exclude any questions already linked to the given
	 * survey. If the survey_id passed in is blank, then
	 * there will be no questions to exclude.
	 * 
	 * @param $survey_id
	 */
	function getQuestionSets($survey_id = 0)
	{
		$query = GroupedQuery::create(SurveyQuestionXref, "", "survey_id");
		if($survey_id)
		{
			$query->constraints = "WHERE survey_question_id NOT IN (SELECT survey_question_id FROM survey_question_xref WHERE survey_id = :survey_id)";
			$query->bind(":survey_id", $survey_id);
		}
		
		$surveyQuestionSets = $query->execute();
		
		return $surveyQuestionSets;
	}
	
	/*
	 * Save questions selected on survey_question_select
	 *
	 * It is possible for the user to select the same question
	 * if that question is part of 2 or more surveys.
	 */
	static function saveSelectedQuestions($questionSelect, $field)
	{
		global $_POST;

		$survey_id = $questionSelect->parent->data->survey_id;

		$questionIds = $_POST["survey_question_xref_id"];

		if(isset($questionIds) AND count($questionIds) > 0)
		{
			$list = implode(",", array_values($questionIds));
			$questions = Query::create(SurveyQuestion, "WHERE survey_question_id IN (SELECT survey_question_id FROM survey_question_xref WHERE survey_question_xref_id IN ($list))")
				->execute();

			if(count($questions) > 0)
			{
				$sort_order = 0;

				foreach($questions as $question)
				{
					$xref = new SurveyQuestionXref();
					$xref->survey_id = $survey_id;
					$xref->survey_question_id = $question->survey_question_id;
					if(!$sort_order)
					{
						$sort_order = $xref->getDefaultSortOrder();
					}
					else
					{
						$sort_order += 1;
					}
						
					$xref->sort_order = $sort_order;
					$xref->save();
				}
			}
		}
		return true;
	}

	/**
	 * 
	 * Given a survey, retrieve the set of SurveyQuestionXref 
	 * objects and renumber the set.
	 */
	static function updateSortOrder($survey)
	{
		$surveyQuestionXrefs = $survey->SurveyQuestionXrefs("ORDER BY sort_order");
		
		// renumber question set
		$idx = 1;
		foreach($surveyQuestionXrefs as $surveyQuestionXref)
		{
			$surveyQuestionXref->saveSortOrder($idx);
			$idx++;
		}
	}
	
	static function deleteSurvey($survey)
	{
		$responses = $survey->Responses();
		if(count($responses) > 0)
		{
			foreach($responses as $response)
			{
				$answers = $response->Answers();
				if(count($answers) > 0)
				{
					foreach($answers as $answer)
						$answer->delete();
				}
				$response->delete();
			}
		}
		$survey->filter = new InclusionFilter("deleted");
		$survey->deleted = true;
		$survey->save();

		return true;
	}
	
} // end SurveyManager


class SurveySendManager
{
	function SurveySendManager()
	{
		
	}

	/*
	 * Called from survey_preview.inc
	 *
	 */
	function sendSurveyEmails($survey)
	{
		$recipients = explode(",", $survey->recipients);
		if(count($recipients) == 0)
			return;

		SurveySendManager::buildEmail($survey, $sender_email, $subject, $surveyMessage);
				
		// send the survey creator a test response copy
		if(array_search($sender_email, $recipients) === FALSE)
			$recipients[] = $sender_email;

		$responses = Query::create(SurveyResponse, "WHERE survey_id=:survey_id")
			->bind(":survey_id", $survey->survey_id)
			->execute();

		if(count($responses) > 0)
			$responses = reindexList($responses, "email");
			
		foreach($recipients as $recipient)
		{
			if(!array_key_exists($recipient, $responses))
			{
				$response = SurveyResponse::generateToken($survey->survey_id, $recipient);
				$response->save();
			}
			else
			{
				$response = $responses[$recipient];
				if(is_array($response)) $response = $response[0];
			}
			SurveySendManager::sendOneSurveyEmail($survey, $subject, $response, $surveyMessage, $sender_email);
		}

		$survey->setStatus("open");
	}
	
	static function buildEmail($survey, &$sender_email, &$subject, &$surveyMessage)
	{
		$subject = Settings::getValue("survey", "email_subject");
		$message_footer = Settings::getValue("survey", "message_footer");

		$sender_email = ($survey->sender_email) ? $survey->sender_email : Survey::getSenderEmail();

		$surveyMessage = $survey->message .= $message_footer;
	}

	static function sendOneSurveyEmail($survey, $subject, $response, $surveyMessage, $sender_email)
	{
		$mergeMgr = new MergeCodeManager($response, $surveyMessage, "SurveyResponse");
		$recipientMessage = $mergeMgr->searchAndReplace($surveyMessage);
			
		$emailHandler = new EmailHandler($response->email, $subject, $recipientMessage, $sender_email);
		return $emailHandler->send();
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
class SurveyCreateManager
{
	var $item;

	/**
	 * Creates a new QuestionnaireManager object.
	 */
	function SurveyCreateManager($survey)
	{
		$this->item = $survey;
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
		$xref = $question->SurveyQuestionXref($this->item->survey_id);
		if($xref)
			return  $xref->sort_order;
	}

	/*
	 * Called by QuestionnaireForm and QuestionSetForm
	 */
	function getQuestions()
	{
		$questions = $this->item->Questions();
		if(count($questions) > 0)
		{
			$idx = 1;
			foreach($questions as $question)
			{
				$question->question_number = $idx;
				$idx++;
			}
		}
		return $questions;
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
		return $this->item->Answers();
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


} // end class SurveyManager


class SurveyResponseManager extends SurveyCreateManager
{
	var $item;

	function SurveyResponseManager($response)
	{
		$this->item = $response;
	}

	function getQuestions()
	{
		$questions = $this->item->Questions();
		if(count($questions) > 0)
		{
			$idx = 0;
			foreach($questions as $question)
			{
				$question->question_number = $idx;
				$idx++;
			}
		}

		return $questions;
	}

	function getAnswers()
	{
		return $this->item->Answers();
	}


	// How resondent is described in spreadsheet view.
	// For some surveys, it will by by full name.
	function getRespondent()
	{
		return $this->item->token;
	}
}


/*
 * Handle getting total results/response counts for one survey for
 * display of results.
 */
class SurveyResultsManager extends SurveyCreateManager
{
	function SurveyResultsManager($survey)
	{
		$this->item = $survey;
	}

	function Responses($constraint = "")
	{
		return $this->item->Responses();
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
}
