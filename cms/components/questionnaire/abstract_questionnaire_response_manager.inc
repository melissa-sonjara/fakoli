<?php
/**\addtogroup questionnaire
 * @{
 */


Fakoli::using("questionnaire");
Fakoli::usingFile("/cms/components/questionnaire/abstract_questionnaire_manager.inc");


/**
 * Defines the methods needed for display of a questionnaire
 * responder's answers to a questionnaire or survey.
 *
 * Instantiating this class requires the questionnaire or
 * survey response object.
 *
 * @author janice
 */
abstract class AbstractQuestionnaireResponseManager extends AbstractQuestionnaireManager
{
	var $item;

	function __construct($item)
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

	/*
	 * Called by QuestionnaireForm function save to save the
	* resonders answers to the questionnaire.
	*
	* Return the name of the DataItem class that stores the answers
	* e.g., "Answer" or "SurveyAnswer"
	*/
	abstract function getAnswerClass();

	function getExcludeHandler()
	{
		return "exclude_response";
	}

	/**
	 * e.g., survey_response_intro
	 *
	 * @return string
	 */
	function getResponseIntroIdentifier()
	{
		$class = codify(strtolower($this->item->prettifyClassName()));
		return "{$class}_intro";
	}
	
	
	/**
	 * e.g., survey_response_form
	 *
	 * @return string
	 */
	function getResponseFormIdentifier()
	{
		return "{$this->item->table}_form";
	}

	function getResetTokenHandler()
	{
		return "reset_token";
	}

	/**
	 * Return the questionnaire or survey obj that the response obj
	 * (this class's "item") is linked to.
	 */
	function getQuestionnaire()
	{
		$response = $this->item;

		if($response->hasRelation(Questionnaire))
		{
			return $response->Questionnaire();
		}
		elseif($response->hasRelation(Survey))
		{
			return $response->Survey();
		}
		else
		{
			Fakoli::end("No relation from response to questionnaire/survey");
		}
	}

	/*
	 * merge code
	 * Called by the response dataitem class
	 * 
	 * @param obj $item - class Survey/Questionnaire 
	 */
	function getEditUrl($item)
	{
		global $config;
		
		$identifier = $this->getResponseFormIdentifier();
		$pk = $item->getPrimaryKey();
		
		// need full url for links placed inside email messages
		$link = "http://". $config["http_host"] ."/". $item->format("{$identifier}?{$pk}={{$pk}}");
		if($this->item->token)
		{
			$link .= "&token={$this->item->token}";
		}	
		return "<a href='$link'>$link</a>";
	}
	
	/*
	 * merge code
	 * Called by the response dataitem class
	 * 
	 * @param obj $item - class Survey/Questionnaire 
	 */
	function getIntroUrl($item)
	{
		global $config;
		
		$identifier = $this->getResponseIntroIdentifier();
		$pk = $item->getPrimaryKey();

		// need full url for links placed inside email messages
		$link = "http://". $config["http_host"] ."/". $item->format("{$identifier}?{$pk}={{$pk}}");
	 	
		return "<a href='$link'>$link</a>";
	}
	
		
	/**
	 * Called by QuestionnaireForm function getIndexedAnswers
	 *
	 * Retrieve answers to the single responder's questionnaire entries.
	 *
	 */
	function getAnswers()
	{
		$pk = $this->item->getPrimaryKey();
		if(!$this->item->$pk)
		{
			return null;
		}
		
		return $this->item->Answers();
	}


	/**
	 * How the respondent should be displayed in the view:
	 * either by name or token.
	 *
	 * The DataItem's default format is used. Override if needed.
	 */
	function getRespondent()
	{
		return $this->item->format();
	}

	/**
	 * reinitialize token for a test user.
	 */
	function resetToken()
	{
		$response = $this->item;
		$responsePk = $response->getPrimaryKey();
		$answerClass = $this->getAnswerClass();

		if(!$response->$responsePk) return;

		$response->setDefaults();

		$answer = new $answerClass();
		$answer->delete("WHERE {$responsePk}={$response->$responsePk}");
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
	 * Instantiate the QuestionnaireResponseManager js class.
	 *
	 * Default action handler name for handling responses.
	 *
	 * TO DO: copy response management code from survey to here.
	 *
	 * For now, this js file is empty.
	 *
	 */
	function writeScript()
	{
		$item = $this->item;

		$questionnaire = $this->getQuestionnaire();
		$itemPk = $questionnaire->getPrimaryKey();
		$item_id = $questionnaire->$itemPk;
		$responsePk = $item->getPrimaryKey();
		$component_name = $this->getComponentName();
		$exclude_handler = $this->getExcludeHandler();
		$response_intro_identifier = $this->getResponseIntroIdentifier();
		$response_form_identifier = $this->getResponseFormIdentifier();
		$reset_token_handler = $this->getResetTokenHandler();

		ob_start();
		?>
<script type="text/javascript" src="/components/questionnaire/js/questionnaire_response.js"></script>
<script type="text/javascript">
var questionnaireMgr;

window.addEvent('domready', function()
{
	questionnaireMgr = new QuestionnaireResponseManager(
		'<?php echo $itemPk ?>', 
		<?php echo $item_id ?>, 
		'<?php echo $responsePk ?>',  
		'<?php echo $component_name ?>',
		'<?php echo $exclude_handler ?>',
		'<?php echo $response_intro_identifier ?>',
		'<?php echo $response_form_identifier ?>',
		'<?php echo $reset_token_handler ?>'
		);
});

</script>
<?
		$script .= ob_get_contents();
		ob_end_clean();
		
		return $script;
	}	
	
} // end AbstractQuestionnaireResponseManager


/**@}*/?>
