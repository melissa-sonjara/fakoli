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

/**
 * SurveyComponentManager - manages survey component; the class name
 * SurveyManager is used for management of the survey feature.
 * 
 * @author janice
 *
 */
class SurveyComponentManager
{
	function __construct()
	{
		
	}
	
	static function enumerateContextHelpClasses($classes)
	{
		$classes[] = "SurveyAnswer";
		$classes[] = "SurveyQuestion";
		$classes[] = "SurveyResponse";
		$classes[] = "Survey";

		return $classes;
	}

	static function setDefaults()
	{
		trace("SurveyManager::setDefaults", 3);

		global $config;

		Settings::setDefaultValue("survey", "sender_email", "", "String", "The email address that will appear as the sender of the survey email", "Survey Emails");
		Settings::setDefaultValue("survey", "email_subject", "Survey from ". $config["http_host"], "String", "The default or standard subject line for survey emails", "Survey Emails");
		Settings::setDefaultValue("survey", "message_footer", "<br><br>Click the link below to access the survey:<br>[survey_link]<br><br>or go to<br>[survey_intro_link], click on [survey_title], and enter your access token [token] at the prompt.<br>", "Text", "The text to be included below the main
			message in all surveys, providing users with a link to response. 
			The text within brackets represent codes that will be filled in with data 
			from the survey record when the survey is sent.", "Survey Emails");
		Settings::setDefaultValue("survey", "interactive_graphs", false, 'Boolean', "Show interactive SVG graphs on the report screens, rather than static images");

	}
	
	static function enumerateDataItemClasses($classes)
	{
		$classes[] = "SurveyAnswer";
		$classes[] = "SurveyQuestion";
		$classes[] = "SurveyResponse";
		$classes[] = "Survey";
		$classes[] = "SurveyQuestionXref";

		return $classes;
	}

	/**
	 * Respond to fired event DeleteUser
	 *
	 * Delete any records in this component that have dependencies on user object.
	 *
	 * @param obj $user - class SiteUser or custom user class
	 */
	static function deleteUser($user)
	{
		$pk = $user->getPrimaryKey();
		$user_id = $user->$pk;
		
		trace("Component survey is deleting objects dependent on user_id {$user_id}", 3);
	
		$tx = new DataTransaction();
		
		try
		{
			$surveys = Query::create('Survey', "WHERE user_id=:user_id")
				->bind(":user_id", $user_id)
				->execute();
			
			foreach($surveys as $survey)
			{
				$survey->joinTransaction($tx);
				SurveyComponentManager::deleteSurvey($survey);
			}
			
			$tx->commit();
		}
		catch(Exception $e)
		{
			$tx->rollback();
			throw $e;
		}
				
		return $user;
	}
	
	static function deleteSurvey($survey)
	{
		$survey_id = $survey->survey_id;
		$questions = $survey->Questions();
		
		$tx = new DataTransaction();
		
		try
		{
			$xref = new SurveyQuestionXref();
			$xref->joinTransaction($tx);
			$xref->delete("WHERE survey_id={$survey_id}");
			
			$responses = $survey->Responses();
			
			if(count($responses))
			{
				foreach($responses as $response)
				{
					$response->joinTransaction($tx);
					SurveyComponentManager::deleteSurveyResponse($response);
				}
			}
			
			$tx->commit();
		}
		catch(Exception $e)
		{
			$tx->rollback();
			throw $e;
		}
		
		// Delete any questions that are now not linked to any survey
		foreach($questions as $question)
		{
			$count = Query::create('SurveyQuestionXref', "WHERE survey_question_id=:survey_question_id")
			->bind(":survey_question_id", $question->survey_question_id)
			->executeValue("COUNT(1)");
			
			if($count == 0)
			{
				$question->delete();
			}		
		}
	}
	
	static function deleteSurveyResponse($response)
	{
		$tx = new DataTransaction();
		
		try
		{
			$response->joinTransaction($tx);
			
			$answer = new SurveyAnswer();
			$answer->joinTransaction($tx);
			$answer->delete("WHERE response_id={$response->response_id}");
			
			$tx->commit();
		}
		catch(Exception $e)
		{
			$tx->rollback();
			throw $e;
		}
	}
	
	static function upgradeComponent($version)
	{
		$mgr = new SurveyUpgradeManager();
		$mgr->upgrade($version);
	}
}
/**@}*/?>
