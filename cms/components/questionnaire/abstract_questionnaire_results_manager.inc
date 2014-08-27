<?php
Fakoli::using("questionnaire");
Fakoli::usingFile("/cms/components/questionnaire/abstract_questionnaire_manager.inc");

/**
 * Defines the methods needed for displaying aggregated
 * results to a survey.
 *
 * Instantiating this class requires the questionnaire or
 * survey object.
 *
 * Create Managers should have the following action handlers:
 *
 *
 * @author janice
 */
abstract class AbstractQuestionnaireResultsManager extends AbstractQuestionnaireManager
{
	var $item;

	function AbstractQuestionnaireResultsManager($item)
	{
		$this->item = $item;
	}

	/**
	 * The name of the questionnaire/survey component
	 */
	abstract function getComponentName();

	/**
	 * The DataItem class name for the Question class
	*/
	abstract function getQuestionClass();

	/**
	 * If questions are linked to the questionnaire/survey through an xref
	 *
	 * Implementations that link questions directly to the questionnaire/survey
	 * will return null.
	*/
	abstract function getXrefClass();

	/**
	 * Returns an array of responses to the questionnaire.
	 * e.g., $questionnaire->Responses. Components may require
	 * special logic for including/excluding responses.
	*/
	abstract function Responses();


	/**
	 *
	 * Given an object of the Response class,
	 * return an instance of this manager class.
	 * e.g., return new QuestionnaireResponseManager($response);
	 * 
	 * If empty object, it is being used by writeScipt
	 * for the purpose of retrieving values like primary key or other settings
	 * in the response manager that don't require a loaded instance.
	*/
	abstract function getResponseManager($response);

	abstract function getResponseClass();
	
	/**
	 * Return the label for the heading on the response list
	 * page data list view. Anonymous surveys may be labeled
	 * "Token" while others will say "Name".
	*/
	abstract function getRespondentColumnHeading();

	/**
	 * Returns the format string or callback for the respondent column.
	 * @return string
	*/
	function getRespondentColumnFormat()
	{
		return "{name}";
	}

	/**
	 * Override this method to provide additional columns to display after the respondent token,
	 * but before the response data.
	 */
	function addRespondentAdditionalColumns($table)
	{
		return $table;
	}

	/**
	 * Return the number of recipients. This is not valid
	 * for all questionnaires/surveys. When no valid, return 0.
	 */
	abstract function getRecipientCount();

	/**
	 * Called by QuestionnaireForm function getIndexedAnswers
	 *
	 * Should return all answers to the survey.
	*/
	function getAnswers()
	{
		return $this->item->Answers();
	}

	function getQuestionNamesHandler()
	{
		return "question_names_dialog";
	}

	function getResponseViewDialog()
	{
		return "response_view";
	}

	/**
	 * How to display on questionnaire_responses results tab.
	 *
	 * Called from the responses list page under Results tab
	 * set.
	 *
	 */
	function formatResponseLink($response)
	{
		$pk = $response->getPrimaryKey();

		$mgr = $this->getResponseManager($response);
		$respondent = $mgr->getRespondent();

		if($response->isSubmitted())
			return $response->format("<a href='#' onclick=\"questionnaireMgr.showResponse('{$pk}', {{$pk}}); return false\">$respondent</a>");
		else
			return $respondent;
	}

	/**
	 * Override if needed.
	 */
	function getResponseViewTitle()
	{
		return $this->item->prettifyClassName() . " Response";
	}
		

	/**
	 * Called by QuestionRenderers from QuestionnaireForm
	 * Given an instance of a question, return its number.
	 * Some renderers need to skip the numbering (headings).
	 */
	function getQuestionNumber($question)
	{
		return QuestionnaireHelper::getQuestionNumber($this->item, $this->getQuestions(), $question);
	}

	/**
	 * Return the number of responders to the questionnaire/survey
	 */
	function getResponseCount()
	{
		return count($this->Responses());
	}

	/**
	 * Shows how many responses received of total; return
	 * empty if not needed.
	 */
	function getProgressBar()
	{
		$percentage = 0;
		$total = $this->getRecipientCount();
		if($total > 0)
		{
			$responseCount = $this->getResponseCount();
			$percentage = round($responseCount / $total * 100, 0);
		}
		if(!$responseCount) $responseCount = 0;

		$html .= $percentage . "%";
		$html .= " <span style='font-size: 8pt'>($responseCount&nbsp;of&nbsp;$total)</span>";
		$html .= "<div class='progress_bar'>\n";
		$html .= "<div class='progress' style='width: $percentage%'>\n";
		$html .= "</div></div>\n";
		return $html;
	}

	function drawEditQuestionNamesButton()
	{
		$pk = $this->item->getPrimaryKey();
		echo $this->item->format("<p><a class='button' href='#' onclick=\"questionnaireMgr.showQuestionNamesDialog({{$pk}});
		return false;\">Edit Column Headings</a></p>\n");
	}

	/**
	* Called by question_names_dialog to build the question names form.
	*/
	function buildQuestionNamesForm()
	{
	$item = $this->item;
	$component = $this->getComponentName();
	$handler = $this->getQuestionNamesHandler();
	$pk = $item->getPrimaryKey();

	$item->filter = new InclusionFilter();
	$form = new AutoForm($item, "POST", "/action/{$component}/{$handler}?$pk={$item->$pk}", "QuestionNames_form");
		$form->ajaxSubmit("function(result) {new questionnaireMgr.questionNamesFormResult(result);}", "function() {document.id('{$form->id}_error').set('text','Failed to communicate with server'); }");

		$questions = $this->getQuestions();
		$table = new DataListView($questions, "QuestionnaireQuestions");
		$table->column("Question", "{question}")
			->column("Question Name", array(QuestionnaireHelper, formatQuestionNameInputField), false)
			;

		$table->emptyMessage = "There are no questions defined.";
		$table->sortable = false;
		$table->cssStyle = "width: 100%";

		$questionSelect = new DataListFieldRenderer($form, $table, "questionnaire_questions");
		$questionSelect->hideLabel = true;
		$form->submitLabel = "Save Question Names";
		$questionSelect->onPostProcess = array($this, saveQuestionNames);

		if(count($questions) == 0)
		{
		$form->readOnlyForm = true;
		}
		return $form;
	}


	/**
	* Save the question names updated through the question_names_dialog.
	*
	*/
	function saveQuestionNames()
	{
		global $_POST;
		$qClass = $this->getQuestionClass();

		foreach($_POST as $name => $value)
		{
			if (!strncmp($name, "question_", 9))
			{
				$id = substr($name, 9);
				checkNumeric($id);
				$question = new $qClass($id);
				$question->question_name = $value;
				$question->filter = new InclusionFilter("question_name");
				$question->save();
			}
		}
		return true;
	}

	/**
	 * From responses data list view, allow survey owner to include/exlude individual
	 * responses from results tabulation.
	 *
	 * @param obj $response
	 * @return string
	 */
	function formatIncludeInResultsLink($response)
	{
		$img = $response->include_in_results ? "on.png" : "off.png";
		$alt = $response->include_in_results ? "include_in_results" : "exclude_from_results";
		$fn = $response->include_in_results ? "excludeResponse" : "includeResponse";

		return $response->format("<a href='#' onclick=\"questionnaireMgr.$fn({response_id}); return false;\"><img src='/fakoli/images/$img' alt='$alt' style='display: inline-block; vertical-align: middle; border: none;'/></a>");
	}
	
	/**
	* Instantiate the QuestionnaireResultsManager js class.
	*
	* Requires the following handlers to exist in the questionnaire component:
	* question_names_dialog
	* response_view
	*
	* response_view_title - title for the response view modal dialog box called
	* from the Results tab responses list page. Standard title is
	* prettified class name.
	*
	*/
	function writeScript()
	{
		$itemPk = $this->item->getPrimaryKey();
		$item_id = $this->item->$itemPk;
		$component_name = $this->getComponentName();
		$question_names_dialog = "question_names_dialog";
		$response_view_dialog = $this->getResponseViewDialog();
		$response_view_title = $this->getResponseViewTitle();
		
		$response_class = $this->getResponseClass();
		$mgr = $this->getResponseManager(new $response_class());
		$exclude_handler = $mgr->getExcludeHandler();
		$responsePk = $mgr->item->getPrimaryKey();

		ob_start();
			?>
<script type="text/javascript" src="/components/questionnaire/js/questionnaire_results.js"></script>
<script type="text/javascript">
var questionnaireMgr;

window.addEvent('domready', function()
{
	questionnaireMgr = new QuestionnaireResultsManager('<?php echo $itemPk ?>', 
		<?php echo $item_id ?>, '<?php echo $component_name ?>', 
		'<?php echo $question_names_dialog ?>',
		'<?php echo $response_view_dialog ?>',
		'<?php echo $response_view_title ?>',
		'<?php echo $exclude_handler ?>',
		'<?php echo $responsePk ?>'
		);
});

</script>
<?
		$script .= ob_get_contents();
		ob_end_clean();
		
		return $script;
	}	
	
} // end AbstractQuestionnaireResultsManager

?>