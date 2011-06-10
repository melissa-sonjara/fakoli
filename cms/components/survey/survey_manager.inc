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
	
	static function enumerateContextHelpClasses($classes)
	{
		array_push($classes, "SurveyAnswer");
		array_push($classes, "SurveyMessage");
		array_push($classes, "SurveyQuestion");
		array_push($classes, "SurveyResponse");
		array_push($classes, "Survey");
		
		return $classes;
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
	
	static function upgradeComponent()
	{
		$mgr = new SurveyUpgradeManager();
		$mgr->upgrade();
	}	
		
} // end class SurveyManager


class SurveyTemplateManager extends SurveyManager
{
	function SurveyTemplateManager($surveyTemplate)
	{
		$this->item = $surveyTemplate;		
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
	
	
	function getRecipientCount()
	{
		return $this->item->getRecipientCount();
	}
	
	function getResponseCount()
	{
		return $this->item->getResponseCount();
	}
	
	/*
	 * Only get answers to submitted (not in progress) questionnaires/surveys
	 */
	function getAnswers()
	{
		return $this->item->Answers();
	}

}
