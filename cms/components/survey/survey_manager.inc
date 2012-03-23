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
class SurveyManager
{
	var $item;

	/**
	 * Creates a new QuestionnaireManager object.
	 */
	function SurveyManager($survey)
	{
		$this->item = $survey;
	}

	function getQuestionKey()
	{
		return "survey_question_id";
	}

	function getAnswerKey()
	{
		return "survey_answer_id";
	}

	function getQuestionNumber($question)
	{
		$xref = $question->SurveyQuestionXref($this->item->survey_id);
		if($xref)
			return  $xref->sort_order;
	}

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

	/**
	 * Returns the field name of the page identifier for
	 * add/update of survey questions.
	 *
	 * @return string
	 */
	function getQuestionForm()
	{
		return "survey_question_form?survey_id={$this->item->survey_id}&survey_question_id=";
	}

	function isEditable()
	{
		return $this->item->isEditable();
	}

	function getPrimaryKey()
	{
		return $this->item->getPrimaryKey();
	}

	/*
	 * Answers to a specific question
	 */
	function getAnswersOneQuestion($question_id)
	{
		return query(SurveyAnswer, "WHERE response_id IN (SELECT response_id FROM survey_response WHERE survey_id = {$this->item->survey_id}) AND survey_question_id={$survey_question_id}");
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

	static function setDefaults()
	{
		trace("SurveyManager::setDefaults", 3);

		global $config;

		Settings::setDefaultValue("survey", "sender_email", "", "String", "The email address that will appear as the sender of the survey email");
		Settings::setDefaultValue("survey", "email_subject", "Survey from ". $config["http_host"], "String", "The default or standard subject line for survey emails");
		Settings::setDefaultValue("survey", "message_footer", "<br><br>Click the link below to access the survey:<br>[survey_link]<br><br>or go to<br>[survey_intro_link], click on [survey_title], and enter your access token [token] at the prompt.<br>", "Text", "The text to be included below the main
			message in all surveys, providing users with a link to response. 
			The text within brackets represent codes that will be filled in with data 
			from the survey record when the survey is sent.");

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

	// $surveyTemplate can be class SurveyTemplate or
	// Survey.
	function createSurveyFromTemplate($surveyTemplate)
	{
		global $user;

		$pk = $user->getPrimaryKey();

		$newTitle = "Copy of {$surveyTemplate->title}";
		do
		{
			$exists = Query::create(Survey, "WHERE title=:title")
			->bind(":title", $newTitle)
			->execute();
			if(count($exists) > 0)
				$newTitle = "Copy of " . $newTitle;

		} while (count($exists) > 0);

		$survey = new Survey();
		$survey->title = $newTitle;
		$survey->introduction = $surveyTemplate->introduction;
		$survey->message = $surveyTemplate->message;
		$survey->sender_email = Survey::getSenderEmail();
		$survey->user_id = $user->$pk;
		$survey->save();

		$questions = $surveyTemplate->Questions();

		if(count($questions) > 0)
		{
			$idx = 1;
			foreach($questions as $question)
			{
				$xref = new SurveyQuestionXref();
				$xref->survey_id = $survey->survey_id;
				$xref->survey_question_id = $question->survey_question_id;
				$xref->sort_order = $idx;
				$xref->save();
				$idx++;
			}
		}
		return $survey;
	}

	function getQuestionSets($survey_id = 0)
	{
		if($survey_id > 0)
		{
			$questions = Query::create(SurveyQuestionXref, "WHERE survey_id=:survey_id")
			->bind(":survey_id", $survey_id)
			->execute();
		}
			
		if(count($questions) > 0)
		{
			$list = formatItems($questions, "{survey_question_id}", ",");
			$constraint .= " AND survey_question_id NOT IN ($list)";
		}

		$surveyQuestionSets = groupedQuery(SurveyQuestionXref, "WHERE survey_id > 0 ". $constraint, "survey_id");
		
		return $surveyQuestionSets;
	}
	
	function getTemplateQuestionSets($survey_template_id = 0)
	{
		if($survey_template_id > 0)
		{
			$questions = Query::create(SurveyQuestionXref, "WHERE survey_template_id=:survey_template_id")
			->bind(":survey_template_id", $survey_template_id)
			->execute();
		}
			
		if(count($questions) > 0)
		{
			$list = formatItems($questions, "{survey_question_id}", ",");
			$constraint .= " AND survey_question_id NOT IN ($list)";
		}

		$surveyQuestionSets = groupedQuery(SurveyQuestionXref, "WHERE survey_template_id > 0 ". $constraint, "survey_template_id");

		return $surveyQuestionSets;	
	}

	/*
	 * Save questions selected on survey_question_select
	 * or survey_template_question_select.
	 *
	 * It is possible for the user to select the same question
	 * if that question is part of 2 or more templates.
	 */
	static function saveSelectedQuestions($questionSelect, $field)
	{
		global $_POST;

		$pk = $questionSelect->parent->data->getPrimaryKey();
		$value = $questionSelect->parent->data->$pk;

		$questionIds = $_POST["survey_question_xref_id"];

		if(isset($questionIds) AND count($questionIds) > 0)
		{
			$list = implode(",", array_values($questionIds));
			$questions = Query::create(SurveyQuestion, "WHERE survey_question_id IN (SELECT survey_question_id FROM survey_question_xref WHERE survey_question_xref_id IN ($list))")
			->bind()
			->execute();

			if(count($questions) > 0)
			{
				$sort_order = SurveyQuestionXref::getDefaultSortOrder($pk, $value);

				foreach($questions as $question)
				{
					$xref = new SurveyQuestionXref();
					$xref->$pk = $value;
					$xref->survey_question_id = $question->survey_question_id;
					$xref->sort_order = $sort_order;
					$xref->save();
					$sort_order++;
				}
			}
		}
		return true;
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


	/*
	 * Called from survey_preview.inc
	 *
	 */
	function sendSurveyEmails($survey)
	{
		global $config;

		$recipients = explode(",", $survey->recipients);

		$subject = Settings::getValue("survey", "email_subject");
		$message_footer = Settings::getValue("survey", "message_footer");

		$sender_email = ($survey->sender_email) ? $survey->sender_email : Survey::getSenderEmail();

		if(count($recipients) == 0)
		return;

		// send the survey creator a test response copy
		$recipients[] = $sender_email;

		$surveyMessage = $survey->message .= $message_footer;

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
				$response = $responses[$recipient];

			SurveyManager::sendOneSurveyEmail($survey, $subject, $response, $surveyMessage, $sender_email);
		}

		$survey->setStatus("open");
	}

	static function sendOneSurveyEmail($survey, $subject, $response, $surveyMessage, $sender_email)
	{
		$mergeMgr = new MergeCodeManager($response, $surveyMessage, "SurveyResponse");
		$recipientMessage = $mergeMgr->searchAndReplace($surveyMessage);
			
		$emailHandler = new EmailHandler($response->email, $subject, $recipientMessage, $sender_email);
		$sent = $emailHandler->send();

		return $sent;
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
		if(!$key)
			$formTitle = "Create a Survey";
		else
		{
			$survey = new Survey($key);
			$formTitle = ($survey->isEditable()) ? "Edit Survey" : "Survey Details";
		}

		$images[0] = "<img src='/fakoli/images/tab_create_survey.gif' style='display: inline-block;vertical-align: middle; border: none'/>&nbsp;";
		$images[1] = "<img src='/fakoli/images/tab_manage_survey.gif' style='display: inline-block;vertical-align: middle; border: none'/>&nbsp;";
		$images[2] = "<img src='/fakoli/images/tab_view_results.gif' style='display: inline-block;vertical-align: middle; border: none'/>&nbsp;";

		$tabs = array(
		$images[0] . $formTitle	=> array(
				"Introduction"			=>	"survey_form",
				"Questions"				=>	"survey_questions",
				"Email"					=>	"survey_email",
				"Preview/Send"			=>	"survey_preview"
				),
					
				$images[1]. "Manage Survey"	=> array(
				"My Survey"				=>	"survey_manage"
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


	static function enumerateContextHelpClasses($classes)
	{
		$classes[] = "SurveyAnswer";
		$classes[] = "SurveyQuestion";
		$classes[] = "SurveyResponse";
		$classes[] = "SurveyTemplate";
		$classes[] = "Survey";

		return $classes;
	}

	static function enumerateDataItemClasses($classes)
	{
		$classes[] = "SurveyAnswer";
		$classes[] = "SurveyQuestion";
		$classes[] = "SurveyResponse";
		$classes[] = "Survey";
		$classes[] = "SurveyTemplate";
		$classes[] = "SurveyQuestionXref";

		return $classes;
	}

	static function upgradeComponent($version)
	{
		$mgr = new SurveyUpgradeManager();
		$mgr->upgrade($version);
	}

} // end class SurveyManager


class SurveyTemplateManager extends SurveyManager
{
	function SurveyTemplateManager($surveyTemplate)
	{
		$this->item = $surveyTemplate;
	}

	function getQuestionNumber($question)
	{
		$pk = $this->item->getPrimaryKey();
		$xref = $question->SurveyQuestionXref($this->item->$pk);
		if($xref)
			return $xref->sort_order;
	}


	/**
	 * Returns the field name of the page identifier for
	 * add/update of survey questions.
	 *
	 * @return string
	 */
	function getQuestionForm()
	{
		return "survey_template_question_form?survey_template_id={$this->item->survey_template_id}&survey_question_id=";
	}

	function isEditable()
	{
		return true;
	}

	function getAnswers()
	{
		return array();
	}

	static function surveyTemplateTabs($key = "", $token = "")
	{
		$tabs = SurveyTemplateManager::getSurveyTemplateTabs();
		$qs = ($key) ? "survey_template_id=$key" : "";
		$tabBar = new TabBar("tabs", $tabs, $qs);

		return $tabBar;
	}

	static function getSurveyTemplateTabs()
	{
		$tabs = array(
				"Survey Template Form"		=>	"survey_template_form",
				"Survey Template Questions"	=>	"survey_template_questions",
				"Survey Template Preview &amp; Publish" => "survey_template_preview"
				);
				return $tabs;
	}
}


class SurveyResponseManager extends SurveyManager
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
class SurveyResultsManager extends SurveyManager
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
