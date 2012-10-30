<?
Fakoli::using("questionnaire");
Fakoli::usingFile("/cms/components/questionnaire/abstract_questionnaire_manager.inc");

class QuestionnaireDashboardManager extends AbstractQuestionnaireDashboardManager
{
	function QuestionnaireDashboardManager($item)
	{
		$this->item = $item; // can be empty instance
	}
	
	function getComponentName()
	{
		return "questionnaire";
	}
	
	function getCreateManager($item)
	{
		return new QuestionnaireCreateManager($item);
	}
	
	function getResultsManager($item)
	{
		return new QuestionnaireResultsManager($item);
	}
	
	function getSendManager($item)
	{
		return null;
	}
	
}

/*
 * Description: provides the more generic questionnaire 
 * classes with the specifics of the questionnaire datamodels and 
 * page identifiers. For example, lets the QuestionSet
 * class know the identifier name of the question form.
 * 
 * @author Janice Gallant for Sonjara, Inc.
 * 
 * 10/13/2010
 */
class QuestionnaireCreateManager extends AbstractQuestionnaireCreateManager
{
	/**
	 * Creates a new QuestionnaireCreateManager object.
	 */
	function QuestionnaireCreateManager($questionnaire)
	{
		$this->item = $questionnaire;
	}
	
	function getQuestionClass()
	{
		return Question;
	}
	
	function getComponentName()
	{
		return "questionnaire";
	}
	
	function getXrefClass()
	{
		return null;
	}
			
	// The class that stores the answers
	function getAnswerClass()
	{
		return "QuestionnaireAnswer";
	}	
	
	static function getTabs($key = "")
	{
		$formTitle = "Create a Questionnaire";
		
		if($key)
		{
			$questionnaire = new Questionnaire($key);
			$formTitle = ($questionnaire->isEditable()) ? "Edit Questionnaire" : "Questionnaire Details";
		}

		$images[0] = "<img src='/fakoli/images/tab_create_survey.png' style='display: inline-block;vertical-align: middle; border: none'/>&nbsp;";
		$images[1] = "<img src='/fakoli/images/tab_view_results.png' style='display: inline-block;vertical-align: middle; border: none'/>&nbsp;";

		$tabs = array(
		$images[0] . $formTitle	=> array(
				"Introduction"			=>	"questionnaire_form",
				"Questions"				=>	"questionnaire_questions",
				"Preview Questionnaire"	=>	"questionnaire_preview"
				),
		
				$images[1] . "View Results"	=> array(
				"Questionnaire Responses"	=>	"questionnaire_responses",
				"Graphical Data"		=>	"questionnaire_graphs",
				"Tabular View"			=>	"questionnaire_data_table",
				"Spreadsheet View"		=>	"questionnaire_spreadsheet"
				),
			);
					
		$qs = ($key) ? "questionnaire_id=$key" : "";
		$tabBar = new TwoLevelTabBar($tabs, $qs);
		
		return $tabBar;
	}

}

/*
 * @response - object of DataItem class that links
 * the set of user's responses.
 */
class QuestionnaireResponseManager extends AbstractQuestionnaireResponseManager
{
	var $item;
	
	function QuestionnaireResponseManager($response)
	{
		$this->item = $response;	
	}
	
	function getComponentName()
	{
		return "questionnaire";
	}
	
	function getQuestionClass()
	{
		return Question;
	}
	

	function getXrefClass()
	{
		return null;
	}	
				
	// The class that stores the answers
	function getAnswerClass()
	{
		return "QuestionnaireAnswer";
	}
	
	/**
	 * Called by questionnaire_response_view action handler
	 */
	function drawResponseViewHeading()
	{
		$response = $this->item;
		echo $response->format("<h2>{Questionnaire.title}</h2>");
		echo "<p><label>Respondent: </label>". $this->getRespondent() . "</p>\n";
		echo $response->format("<p><label>Date Submitted: </label>{date_submitted:F d, Y}</p>\n");
	}
	
	static function formatManagerResponseLink($response)
	{
		$mgr = new QuestionnaireResponseManager($response);
		return $mgr->formatResponseLink();
	}	
		
}


/*
 * Handle getting total results/response counts for one survey for
 * display of results.
 */
class QuestionnaireResultsManager extends AbstractQuestionnaireResultsManager
{
	function QuestionnaireResultsManager($questionnaire)
	{
		$this->item = $questionnaire;
	}
	
	function getComponentName()
	{
		return "questionnaire";
	}
	
	function getQuestionClass()
	{
		return Question;
	}	
	
	/**
	 * Called by Spreadsheeet View
	 * 
	 * @param $constraint
	 */
	function Responses($constraint = "")
	{
		return $this->item->Responses();
	}

	function getAnswerClass()
	{
		return "QuestionnaireAnswer";
	}
	
	function getResponseManager($response)
	{
		return new QuestionnaireResponseManager($response);
	}

	function getXrefClass()
	{
		return null;
	}	
	
	/**
	 * For spreadsheet view
	 */
	function getRespondentColumnHeading()
	{
		return "Respondent";
	}
	
	function getRecipientCount()
	{
		return 0;
	}
}
?>