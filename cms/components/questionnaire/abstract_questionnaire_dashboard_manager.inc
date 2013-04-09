<?php
Fakoli::using("questionnaire");

/**
 * Manages options from questionnaire list or dashboard page.
 *
 * @author janice
 *
 */
abstract class AbstractQuestionnaireDashboardManager extends AbstractQuestionnaireManager
{
	var $item;

	function AbstractQuestionnaireDashboardManager($item)
	{
		$this->item = $item;
	}

	/**
	 * The name of the questionnaire/survey component
	 */
	abstract function getComponentName();

	/**
	 *
	 * Given an object of the Questionnaire class,
	 * return an instance of the create manager class.
	 * e.g., return new QuestionnaireCreateManager($item);
	*/
	abstract function getCreateManager($item);

	abstract function getResultsManager($item);

	// can return null
	abstract function getSendManager($item);

	/**
	 * The title field used for the clone dialog.
	*/
	function getTitleField()
	{
		return "title";
	}

	/**
	 * The fields in the item that should be copied into a cloned
	 * copy.
	 */
	function getCloneFields()
	{
		return null;
	}

	/**
	 * Creates a default name for the questionnaire form
	 * or detail first tab from the item class name.
	 * 
	 * Must be abstract because dashboard manager can
	 * be instantied without an item.
	 *
	 * E.g., class name Survey default name:
	 * survey_form
	 */
	function getQuestionnaireFormIdentifier()
	{
		$class = codify(strtolower($this->item->prettifyClassName()));
		return "{$class}_form";
	}
	

	/**
	 * Sets the defaults paths used in the js QuestionnaireCreate class
	 * The default is based on the codified, prettified class name of
	 * the object.
	 *
	 * Classes should override if they have different names.
	 *
	 * Default Names:
	 * clone dialog - the dialog that clones a questionnaire (e.g., clone_questionnaire_dialog);
	 * question list identifier - the identifier of the question list page (e.g., questionnaire_questions)
	 * question_form_identifier - the identifier of the questionnaire form page which
	 * 		is the page to be directed to after clone
	 * item_delete_handler - the handler that can delete the item (e.g., questionnaire_delete)
	 */
	function getCloneDialog()
	{
		$class = codify(strtolower($this->item->prettifyClassName()));
		return "clone_{$class}_dialog";
	}

	function getQuestionnaireViewDialog()
	{
		$class = codify(strtolower($this->item->prettifyClassName()));
		return "{$class}_view";
	}


	/**
	 * Creates a default name for the handler that
	 * would be called from a questionnaire list
	 * page to delete the questionnaire.
	 *
	 * E.g., class name Survey results in delete
	 * handler name survey_delete
	 *
	 */
	function getQuestionnaireDeleteHandler()
	{
		$class = codify(strtolower($this->item->prettifyClassName()));
		return  "{$class}_delete";
	}

	function getResponseFormIdentifier()
	{
		$class = codify(strtolower($this->item->prettifyClassName()));
		return  "{$class}_response_form";
	}

	function getResultsPageIdentifier()
	{
		$class = codify(strtolower($this->item->prettifyClassName()));
		return  "{$class}_responses";
	}

	function buildCloneForm()
	{
		$item = $this->item;
		$title_field = $this->getTitleField();
		$itemPk = $item->getPrimaryKey();
		$cloneDialog =  $this->getCloneDialog();
		$component_name = $this->getComponentName();

		$item->$title_field = preg_replace("/'/","''", $item->$title_field);

		do
		{
			$item->$title_field = "Copy of " .$item->$title_field;
		} while ($item->exists("WHERE {$title_field}='{$item->$title_field}'"));

		$item->title = preg_replace("/''/","'", $item->$title_field);

		$item->filter = new InclusionFilter($title_field);
		$form = new AutoForm($item, "POST", "/action/{$component_name}/{$cloneDialog}?{$itemPk}={$item->$itemPk}", "CloneQuestionnaire_form");
		$form->ajaxSubmit("function(result) {questionnaireMgr.cloneQuestionnaireResult(result);}", "function() {\$('{$form->id}_error').set('text','Failed to communicate with server'); }");
		$form->required($title_field);
		$form->unique($title_field);
		$form->button("Cancel", "questionnaireMgr.closeDialog()", null, true);

		return $form;
	}


	function getDashboardActions()
	{
		return array(
				""					=>	"",
				"view"				=>	"View",
				"edit"				=>	"Edit",
				"public_view"		=>	"Respondent's View",
				"clone"				=>	"Save As...",
				"view_results"		=>	"View Results",
				"delete"			=>	"Delete",
		);

	}


	/**
	 * Copies the manager instance's questionnaire item from destination (dst).
	 *
	 * @param Array $copyFields - the array of fields of the Questionnaire
	 * item that should be copied (e.g., title)
	 *
	 */
	function cloneQuestionnaire()
	{
		$mgr = $this->getCreateManager($this->item);
		return $mgr->cloneQuestionnaire($this->getTitleField(), $this->getCloneFields());
	}


	/*
	 *
	* Called from dashboard data list view.
	* Actions:
	* View
	* Edit
	* Clone
	*
	* @param unknown_type $item
	*
	*  Child classes may with to implement additional actions
	*/
	function formatActions($item)
	{
		$mgr = $this->getResultsManager($item);
		$pk = $item->getPrimaryKey();
		
		$responseCount = $mgr->getResponseCount();

		$mgr = $this->getCreateManager($item);
		$actions = $this->getDashboardActions();
		$mgr->filterActions($actions);

		$sendMgr = $this->getSendManager($item);
		if($sendMgr)
		{
			$sendActions = $sendMgr->getSendActions();
			if(count($sendActions))
			{
				foreach($sendActions as $code => $name)
				{
					$actions[$code] = $name;
				}
			}	
		}

		if($responseCount == 0)
		{
			unset($actions["view_results"]);
		}

		$itemPk = $item->getPrimaryKey();

		$html .= $item->format("<span id='action_result_{{$pk}}' class='dashboard_action_result'></span><select id='action_{{$pk}}' style='width: 80px' onchange=\"questionnaireMgr.handleQuestionnaireAction(this, {{$pk}}); return false;\">");

		foreach($actions as $identifier => $label)
		{
			$value = htmlSafe($identifier);
			$html .= "<option value='$value'>$label</option>";
		}
		$html .= "</select>\n";

		return $html;
	}

	function formatTitleLink($item)
	{
		$form_id = $this->getQuestionnaireFormIdentifier($item);
		$itemPk = $item->getPrimaryKey();
		$title_field = $this->getTitleField();

		return $item->format("<a href=\"/{$form_id}?$itemPk={$item->$itemPk}\">{{$title_field}}</a>\n");
	}
	
	/**
	 * From survey dashboard, instantiate the results manager and 
	 * get the progress bar.
	 * 
	 * @param obj $item - class Questionnaire or Survey
	 * @return string
	 */
	function getProgressBar($item)
	{
		$resultsMgr = $this->getResultsManager($item);
		return $resultsMgr->getProgressBar();
	}

	/**
	 * Instantiate the QuestionnaireDashboardManager js class.
	 *
	 * References following handlers to exist in the questionnaire component:
	 * clone_[item_class]_dialog
	 * [item_class]_delete
	 * [item_class]_view
	 *
	 * The var $item is an empty instance of the questionnaire class.
	 */
	function writeScript()
	{
		$item = $this->item;
		$itemPk = $item->getPrimaryKey();
		$component_name = $this->getComponentName();
		$item_label = $item->prettifyClassName();
		$clone_dialog = $this->getCloneDialog();
		$questionnaire_form_identifier = $this->getQuestionnaireFormIdentifier();
		$item_delete_handler = $this->getQuestionnaireDeleteHandler();
		$item_view_dialog = $this->getQuestionnaireViewDialog();
		$response_form_identifier = $this->getResponseFormIdentifier();
		$results_page_identifier = $this->getResultsPageIdentifier();

		$sendMgr = $this->getSendManager($this->item);
		if($sendMgr)
		{
			$script .= $sendMgr->writeScript();
		}

		ob_start();
		?>
<script type="text/javascript" src="/components/questionnaire/js/questionnaire_dashboard.js"></script>
<script type="text/javascript">

var questionnaireMgr;

window.addEvent('domready', function()
{
	questionnaireMgr = new QuestionnaireDashboardManager(
		'<?php echo $itemPk ?>', 
		'<?php echo $component_name ?>', 
		'<?php echo $item_label ?>',
		'<?php echo $clone_dialog ?>', 
		'<?php echo $questionnaire_form_identifier ?>', 
		'<?php echo $item_delete_handler ?>',
		'<?php echo $item_view_dialog ?>',
		'<?php echo $response_form_identifier ?>',
		'<?php echo $results_page_identifier ?>');

	if(questionnaireSendMgr)
	{
		questionnaireMgr.questionnaireSendMgr = questionnaireSendMgr;
	}
});

</script>
<?
		$script .= ob_get_contents();
		ob_end_clean();
		
		return $script;
	}	
} // AbstractQuestionnaireDashboardManager
