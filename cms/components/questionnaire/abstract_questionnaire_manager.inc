<?php

/**
 * Contains methods used by all 3 classes
 * 
 * @author janice
 *
 */
class AbstractQuestionnaireManager
{
	function AbstractQuestionnaireManager()
	{
	}
	
	/**
	 * Retrieves questions through the questionnaire or survey
	 * DataItem object.
	 * 
	 * Since this method is needed for all 3 managers, classes that
	 * extend should call shared static method
	 * 
	 */
	function getQuestions()
	{
		$sort_field = $this->getSortField();
		$questions = $this->item->Questions("ORDER BY $sort_field");
		return QuestionnaireHelper::setQuestionNumbers($this->item, $questions);
	}
	
	/**
	 * Retrieves the table that has the question sort order field
	 * either Question or a QuestionXref
	 * 
	 * If the item should have a relation to retrieve the
	 * questions or xrefs that is the pluralized version of the class
	 * name.
	 * e.g., SurveyQuestionXrefs or Questions
	 * 
	 * If not, override this function.
	 */
	function getSortableItems()
	{
		if($this->getXrefClass())
		{
			$relation =  pluralize($this->getSortableClass());
		}
		else
		{
			$relation = "Questions";
		}
		$sort_field = $this->getSortField();

		return $this->item->$relation("ORDER BY $sort_field");
	}
	
	function getSortableClass()
	{
		return ($this->getXrefClass()) ? $this->getXrefClass() : $this->getQuestionClass();
	}
	
	function getSortableObj()
	{
		$class = $this->getSortableClass();
		return new $class();
	}
	
	function getSortField()
	{
		$obj = $this->getSortableObj();
		return ($obj->hasField("sort_order")) ? "sort_order" : "question_number";
	}
	
	/**
	 * Returns the primar key name of the question obj
	 */
	function getQuestionKey()
	{
		$qClass = $this->getQuestionClass();
		$question = new $qClass();
		return $question->getPrimaryKey();
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

	function getAnswerKey()
	{
		$class = $this->getAnswerClass();
		$obj = new $class();
		return $obj->getPrimaryKey();
	}
	
	function getPrimaryKey()
	{
		return $this->item->getPrimaryKey();
	}
		
}

/**
 * Questionnaire/Survey Implementation instructions
 * 
 * Components that interface with Questionnaire must meet the following
 * requirements
 * 
 * 1) DataItem classes
 * Item class - the class that can retrieve Questions and Answers.
 * Should extend AbstractQuestionnaire
 * 
 * Question class - the class that contains question data and links
 * to the item class either directly through a foreign key or through
 * an xref table. Should extend AbstractQuestion. Question data items must
 * contains:
 * question
 * question_name
 * question_type_id
 * options
 * required
 * char_limit
 * num_rows
 * context_help
 * 
 * Response class - the class that links to both the Questionnaire item and the
 * Answers. Extend AbstractQuestionnaireResponse class. This class should
 * contain a default format for how a respondent should appear in the results
 * display (by token or name)
 * 
 * Answer class - the class that links a question to its answer and to its
 * respondent. Extend AbstractQuestionnaireAnswer
 * 
 * 2) Manager classes:
 * Implementing components should also have the following classes modeled
 * on Questionnaire manager classes but extending the abstract class.
 * 
 * [MyClassName]DashboardManager extends AbstractQuestionnaireDashboardManager (optional)
 * [MyClassName]CreateManager extends AbstractQuestionnaireCreateManager
 * [MyClassName]ResponseManager extends AbstractQuestionnaireResponseManager
 * [MyClassName]ResultsManager extends AbstractQuestionnaireResultsManager
 * 
 * 3) Handler files:
 * Implementing components may need the following handler files in their
 * component:
 * 
 * clone_[my_item_class]_dialog - create a copy of a questionnaire/survey - requires dashboard manager
 * question_delete - if no xref 
 * question_remove - if xref table
 * question_names_dialog - if results are displayed in spreadsheet
 * [my_item_class]_delete - for delete from list or dashboard page - requires dashboard manager
 * reorder_questions - for draggable reorder from question list page
 * response_view - to display one respondent's answers in a modal dialog
 */



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
	 * E.g., class name Survey results in list page
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
		$responseCount = $mgr->getResponseCount();
		
		$mgr = $this->getCreateManager($item);
		$actions = $mgr->getActions();
		
		if($responseCount == 0)
		{
			unset($actions["view_results"]);
		}
	
		$itemPk = $item->getPrimaryKey();
		
		$html = $item->format("<select style='width: 80px' onchange=\"questionnaireMgr.handleQuestionnaireAction(this, {$item->$itemPk}); return false;\">");
		
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
		$form_id = $this->getQuestionnaireFormIdentifier();
		$itemPk = $item->getPrimaryKey();
		$title_field = $this->getTitleField();
		
		return $item->format("<a href=\"/{$form_id}?$itemPk={$item->$itemPk}\">{{$title_field}}</a>\n");	
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
			$setSendMgr = "questionnaireMgr.questionnaireSendMgr = questionnaireSendMgr;";
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
});

		<?php echo $setSendMgr ?>
</script>
<?
		$script .= ob_get_contents();
		ob_end_clean();
		
		return $script;
	}	
}

/**
 * Optional - for surveys/questionnaires that send requests for
 * responses via email.
 * 
 * NOTE: this class is not tested as it has not yet been implemented.
 * 
 * To use this class, response DataItem class must have the following fields:
 * message
 * 
 * optional fields:
 * include_in_results - default true - if have this field, must have function setExcluded
 * 	that sets a response object to not be included in results.
 * user_id (but if user_id, must have relation "Author")
 * sender_email
 * cc_recipients - defaults to not included
 * subject - defaults to settings email_subject
 * additional_recipients - to send a subsequent batch
 * 
 * methods questionnaire/survey DataItem class should have:
 * getNonResponders
 * 
 * Relations:
 * Author
 * 
 * @author janice
 *
 */
abstract class AbstractQuestionnaireSendManager extends AbstractQuestionnaireManager
{
	var $item;
	var $recipients;
	var $responses = array();
	var $excludes = array();
	var $addl_recipients = array();
	var $msg;
	var $nonResponsiveCount;
	
	function AbstractQuestionnaireSendManager($item)
	{
		$this->item = $item;
	}
	
	abstract function getComponentName();
	
	/**
	 * The DataItem class that stores the responses
	 * (e.g., QuestionnaireResponse)
	 */
	abstract function getResponseClass();
	
	abstract function getResultsManager();
	
	function getSendTestEmailDialog()
	{
		return "send_test_email_dialog";
	}
	
	function getSendTestEmailHandler()
	{
		return "send_test_email";
	}
	
 	function getReminderDialog()
 	{
  		return "reminder_dialog";
 	}
 	
	function getRecipientsDialog()
	{
		return "recipients_dialog";
	}

	function getOpenHandler()
	{
		$class = codify(strtolower($this->item->prettifyClassName()));
		return "open_{$class}"; 
	}
	
	function getCloseHandler()
	{
		$class = codify(strtolower($this->item->prettifyClassName()));
		return "close_{$class}"; 
	}
	
	function getSendPageIdentifier()
	{
		$class = codify(strtolower($this->item->prettifyClassName()));
		return "{$class}_preview"; 
	}
	
	function getSenderEmail()
	{
		$item = $this->item;
		
		global $user;
		
		if($item->hasField("sender_email") && $item->sender_email)
		{
			return $item->sender_email;
		}
		
		$email = Settings::getValue("survey", "sender_email");
	
		if(!$email)
		{
			$email = $user->email;
		}
	
		return $email;	
	}
	
	/**
	 * Get the recipients from the following fields, if in the
	 * field list for the item:
	 * recipients (required)
	 * cc_recipients (optional)
	 * sender_email (optional)
	 * user_id (optional)
	 */
	function getAllRecipients()
	{
		$item = $this->item;
		$recipients = explode(",", $item->recipients);
		
		if($item->hasField("sender_email") && $item->sender_email)
		{
			if(array_search($item->sender_email, $recipients === FALSE))
			{
				$recipients[] = $item->sender_email;
			}
			$this->excludes[] = $item->sender_email;
		}
		
		if($item->hasField("user_id") && $item->user_id)
		{
			$user = $item->Author();
			if(array_search($user->email, $recipients) === FALSE)
			{
				$recipients[] = $user->email;
				$this->excludes[] = $user->email;
			}
		}
		
		if($item->hasField("cc_recipients") && $item->cc_recipients)
		{
			$ccs = explode(",", $item->cc_recipients);
			foreach($ccs as $cc)
			{
				$recipients[] = $cc;
				$this->excludes[] = $cc;
			}
		}
		
		return $recipients;
	}
	
	/**
	 * Given an array of email addresses, either find an existing
	 * response obj or create a new one.
	 * 
	 * @param $recipients
	 */
	function generateResponses($recipients)
	{		
		$item = $this->item;
		$responseClass = $this->getResponseClass();
		$itemPk = $this->getPrimaryKey();
		$responses = array();
	
		$indexedResponses = IndexedQuery::create($responseClass, "WHERE {$itemPk}=:{$itemPk}", "email")
			->bind(":{$itemPk}", $item->$itemPk)
			->execute();

		foreach($recipients as $recipient)
		{
			if(!array_key_exists($recipient, $responses))
			{
				$response = $this->generateToken($recipient);
				$response->save();
			}
			else
			{
				$response = $indexedResponses[$recipient];
			}
			$responses[] = $response;
		}	
		
		return $responses;
	}
	
	/*
	 * generateToken
	 *
	 * Create a token to grant access to enter responses to
	 * the questionnaire/survey. Before accepting a generated
	 * token, we need to make sure that that token does not
	 * exist already.
	 */
	function generateToken($email)
	{
		$surveyResponse = null;
		$itemPk = $this->item->getPrimaryKey();
		$responseClass = $this->getResponseClass();
		
		if(!$response)
		{
			$response = new $responseClass();

			do
			{
				$code = "";

				for($i = 0; $i < 5; ++$i)
				{
					$code .= chr(rand(ord('A'), ord('Z')));
				}
					
				$response->token = $code;
				// Set any initial default fields (e.g., status)
				$response->setDefaults();
				$response->$itemPk = $this->item->$itemPk;
				$response->email = $email;
			}
			while(!$response->insert());
		}

		return $response;
	}
	
	/**
	 * Set certain responses to be excluded from results. For example,
	 * the survey sender or author. If the survey has a cc_recipients field,
	 * set those to excluded.
	 */
	function setExcluded()
	{
		$item = $this->item;
		
		if(!$item->hasField("include_in_results"))
		{
			return;
		}
		
		$indexedResponses = $this->reindexList($this->responses, "email");
		
		if(!count($this->excludes))
		{
			return;
		}
		
		foreach($this->excludes as $excludeEmail)
		{
			if(!array_key_exists($excludeEmail, $indexedResponses)) continue;
			
			$response = $indexedResponses[$excludeEmail];
			$response->setExcluded();
		}
	}
	
	function sendToRecipients()
	{	
		if(!$item->recipients)
		{
			return;
		}
		
		$recipients = $this->getAllRecipients();
		
		$this->responses = $this->generateResponses($recipients);
		
		$this->setExcludes();
		
		if(!count($this->responses))
		{
			return;
		}

		$this->sendEmails();
	}
	
	/**
	 * When sending a 2nd batch, don't send to sender, author, or
	 * cc recipients.
	 */
	function sendToAdditional()
	{
		if(!$item->recipients)
		{
			return;
		}
	
		if($item->hasField("additional_recipients") && $item->additional_recipients)
		{
			$addl_recipients = explode(",", $item->additional_recipients);
			$addl_recipients = array_unique($addl_recipients);
			$addl_recipients = array_diff($addl_recipients, $recipients);
			if(!count($addl_recipients))
			{
				return;
			}
			$recipients = $addl_recipients;
			$this->excludes = $recipients;
			$this->addl_recipients = $recipients;
		}
		else
		{
			return;
		}
		
		$this->responses = $this->generateResponses($recipients);
		
		$this->setExcludes();
		
		if(!count($this->responses))
		{
			return;
		}

		$rtn = $this->sendEmails();
		
		$this->moveAdditionalToRecipientList();
		
		return $rtn;
	}
	
	
	/**
	 * Given a survey and a set of recipients, create a response
	 * record if does not exist for each email already and send the email
	 * to all recipients.
	 * 
	 * @param obj $survey
	 * @param array $recipients
	 */
	function sendEmails()
	{	
		$item = $this->item;
		
		$message = $this->getMessage();
		$subject = $this->getSubject();
		
		$sender_email = $this->getSenderEmail();
		
		foreach($this->responses as $response)
		{
			$rtn = $this->sendOneEmail($response->email, $subject, $message, $sender_email);
		}
		
		$this->moveAdditionalToRecipientList();	
		
		return $rtn;
	}
	
	function getSubject()
	{
		$item = $this->item;
		
		if($item && $item->hasField("subject") && $item->subject)
		{
			return $item->subject;
		}
		
		return Settings::getValue("survey", "email_subject");		
	}
	
	function getMessage()
	{
		$footer = Settings::getValue("survey", "message_footer");
		$message = $this->item->message;
		
		if($footer)
		{
			$message .=  "\r\n\r\n{$footer}";
		}
		
		return $message;
	}
	
	function sendOneEmail($email, $subject, $message, $sender_email)
	{
		$responseClass = $this->getResponseClass();
		
		$mergeMgr = new MergeCodeManager($response, $message, $responseClass);
		$recipientMessage = $mergeMgr->searchAndReplace($message);
			
		$emailHandler = new EmailHandler($response->email, $subject, $recipientMessage, $sender_email);
		return $emailHandler->send();
	}	
	
	/*
	* Add the set of additional recipients to the existing recipients
	* field values. We don't use the item's field value "additional_recipients"
	* because that may not match the final list which excludes duplicates.
	*/
	function moveAdditionalToRecipientList()
	{
		$item = $this->item;
		
		if(!$item->hasField("additional_recipients"))
		{
			return;
		}
		
		if(!count($this->addl_recipients))
		{
			return;
		}

		$recipients = explode(",", $item->recipients);
		foreach($this->addl_recipients as $ar)
		{
			$recipients[] = $ar;
		}
		
		$item->recipients = implode(",", $recipients);
		$item->additional_recipients = '';
		$item->filter = new InclusionFilter("recipients", "additional_recipients");
		$item->save();
	}

	
	/**
	 * Send a test email to the specified email addresses. 
	 * The email is excluded from results.
	 *
	 * @param obj $survey
	 * @param String $recipients
	 */
	function sendTests($recipients)
	{
		$recipients = explode(",", $recipients);
		$recipients = array_unique($recipients);
		
		$this->excludes = $recipients;
		
		$this->responses = $this->generateResponses($recipients);
		
		$this->setExcludes();
		
		if(!count($this->responses))
		{
			return;
		}

		$this->sendEmails();
		
		return $this->sendEmails();		
	}
	
	/**
	 * Get the list of recipients who have not responded.
	 */
	function getNonResponsives()
	{
		
	}
	
	/**
	 * Called from preview/send page
	 * 
	 * @param Array $nonResponsives - recipients who have not yet responded.
	 */
	function sendReminderEmails()
	{
		if(!count($this->recipients))
		{
			$this->msg = "Send Reminders: there are no recipients that have not responded.";
			return;
		}
	
		$this->responses = $this->generateResponses($this->recipients);
		
		if(!count($this->responses))
		{
			$this->msg = "Send Reminders: no response objects generated.";
			return;
		}

		$sent = $this->sendEmails();
		
		return ($sent) ? "OK" : "FAIL";	
	}
	
	/**
	 * Called from sent_test_email_dialog
	 */
	function buildSendTestEmailForm()
	{
		$item = $this->item;
		$component_name = $this->getComponentName();
		$dialog = $this->getSendTestEmailDialog();
		$itemPk = $this->getPrimaryKey();
		
		$email = new EmailTemplate();
		$email->filter = new InclusionFilter("recipients");
		$email->recipients = $user->email;

		$form = new AutoForm($email, "POST", "/action/{$component_name}/{$dialog}?{$itemPk}={$item->$itemPk}", "SendTestEmail_form");
		$form->ajaxSubmit("function(result) {questionnaireSendMgr.testEmailResult(result);}", "function() {\$('{$form->id}_error').set('text','Failed to communicate with server'); }");
		$form->required("recipients");
		$recipientRenderer = new EmailListFieldRenderer($form, "recipients");
		$recipientRenderer->removeDuplicates = true;
		$form->submitLabel = "Send Test Email";
		$form->button("Cancel", "questionnaireSendMgr.closeDialog()", null, true);
		$form->annotate("recipients", "Recipients of a test email will receive a valid response token but their responses will not be counted in the results. (separate email addresses with a comma ',')");
		
		return $form;
	}
	
	/**
	 * Called from send_reminder_dialog handler.
	 */
	function buildReminderForm()
	{
		$item = $this->item;
		$mgr = $this->getResultsManager();
		$nonResponsives = $questionnaire->getNonResponders();
		$this->nonResponsiveCount = count($nonResponsives);
		$questionnaire->recipients = formatItems($nonResponsives, "{token}", ", ");
		// Set the recipient list for use when form is submitted
		$this->recipients = formatItems($nonResponsives, "{email}", ",");

		$item->filter = new InclusionFilter("title", "recipients", "sender_email", "message", "subject");
		$form = new AutoForm($item, "POST", "/action/questionnaire/questionnaire_reminder_form?questionnaire_id=$questionnaire_id", "QuestionnaireReminder_form");
		$form->ajaxSubmit("function(result) {questionnaireSendMgr.reminderDialogResult(result);}", "function() {\$('{$form->id}_error').set('text','Failed to communicate with server'); }");

		$form->getRenderer("message")->rows = 10;
		$form->submitLabel = "Send Reminders";
		$form->annotate("message", "You can use the email template message as is or customize it for this reminder.");
		$form->readOnly("sender_email", "recipients", "title");
		$form->required("message", "subject");
		$form->button("Cancel", "questionnaireSendMgr.closeDialog()", null, true);
		$form->alias("recipients", "Recipient Tokens");
		
		return $form;
	}
	
	/**
	 * Instantiate the QuestionnaireSendManager js class.
	 * 
	 * References the following handlers to exist in the questionnaire component.
	 * Components must either create these handlers or not call the functions that
	 * call them.
	 * 
	 * open_[item_class]
	 * close_[item_class]
	 * send_test_email_dialog
	 * send_test_email
	 * 
	 * The var $item is an empty instance of the questionnaire class.
	 */
	function writeScript()
	{
		$item = $this->item;
		$itemPk = $item->getPrimaryKey(); 
		$component_name = $this->getComponentName();
		$item_label = $item->prettifyClassName();
		$send_test_email_dialog = $this->getSendTestEmailDialog();
		$send_test_email_handler = $this->getSendTestEmailHandler();
		$open_handler = $this->getOpenHandler();
		$reminder_dialog = $this->getReminderDialog();
		$close_handler = $this->getCloseHandler();
		$send_page_identifier = $this->getSendPageIdentifier();
		$recipients_dialog = $this->getRecipientsDialog();
	
		ob_start();
?>
<script type="text/javascript" src="/components/questionnaire/js/questionnaire_send.js"></script>
<script type="text/javascript">
var questionnaireSendMgr;

window.addEvent('domready', function()
{
	questionnaireSendMgr = new QuestionnaireSendManager(
		'<?php echo $itemPk ?>', 
		'<?php echo $component_name ?>', 
		'<?php echo $item_label ?>',
		'<?php echo $send_test_email_dialog ?>',
		'<?php echo $send_test_email_handler ?>',
		'<?php echo $open_handler ?>',
		'<?php echo $close_handler ?>',
		  '<?php echo $reminder_dialog ?>',
		  '<?php echo $send_page_identifier ?>',
		  '<?php echo $recipients_dialog ?>');
	
});
</script>
<?
		$script .= ob_get_contents();
		ob_end_clean();
		
		return $script;
	}		
}

/*
 * Provides an interface between a custom questionnaire 
 * component and the QuestionnaireForm and Question Renderers
 * and Results View classes. Revies an object of class
 * Question or Survey or other DataItem class that can
 * access the questions and answers.
 * 
 * This class defines the methods that are required for components that
 * use the QuestionnaireForm for the user survey-create interface.
 * 
 * Instantiating this class requires the questionnaire or
 * survey object.
 */
abstract class AbstractQuestionnaireCreateManager  extends AbstractQuestionnaireManager
{
	var $item;				//<	The questionnaire or survey object
	
	function AbstractQuestionnaireCreateManager($item)
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
	 * The title field used for the clone dialog.
	 */
	function getTitleField()
	{
		return "title";	
	}
	
	/**
	 * Called by QuestionnaireForm function getIndexedAnswers
	 * 
	 * The CreateManager should return null because this manager is used
	 * to display a preview of the questionnaire the user created, not
	 * the answers. The answers will not be display so it would be
	 * a wasted query to retrieve them.
	 */
	function getAnswers()
	{
		return null;
	}

	function getQuestionDeleteHandler()
	{
		return "question_delete";
	}
	
	function getQuestionRemoveHandler()
	{
		return "question_remove";
	}
	
	function getReorderHandler()
	{
		return "reorder_questions";
	}
	
	/**
	 * Creates a default name for the questionnaire list page
	 * from the item class name.
	 * 
	 * E.g., class name Survey results in list page
	 * survey_questions
	 */
	function getQuestionListIdentifier()
	{
		$class = codify(strtolower($this->item->prettifyClassName()));
		return "{$class}_questions";
	}
	
	/**
	 * Creates a default name for the questionnaire question form
	 * page.
	 * 
	 */
	function getQuestionForm()
	{
		$class = codify(strtolower($this->item->prettifyClassName()));
		return "{$class}_question_form";
	}
	
	/**
	 * From question list page, format the link to edit the question.
	 * 
	 * @param $question
	 */
	function formatQuestionLink($question)
	{
		$question_form_id = $this->getQuestionForm();
		$qPk = $question->getPrimaryKey();
		
		return $question->format("<a href='{$question_form_id}?{$qPk}={{$qPk}}'>{question}</a>\n");		
	}
	
	/**
	 * Copies the manager instance's questionnaire item from destination (dst). 
	 * 
	 * @param Array $copyFields - the array of fields of the Questionnaire
	 * item that should be copied (e.g., title)
	 * 
	 */
	function cloneQuestionnaire($title_field, $cloneFields = array())
	{
		$src = $this->item;
		$itemClass = get_class($src);
		$dst = new $itemClass();
	
		$dst->setDefaults();
		$dst->$title_field = $src->$title_field;
	
		if(count($cloneFields) > 0)
		{
			foreach($cloneFields as $field)
			{
				$dst->$field = $src->$field;
			}
		}
			
		$dst->save();
	
		$questions = $this->getQuestions();
		if(count($questions) == 0) return;
		
		$qPk = $questions[0]->getPrimaryKey();
		$itemPk = $dst->getPrimaryKey();
		
		foreach($questions as $question)
		{
			$qs = clone $question;
			$qs->$qPk = null;
			$qs->$itemPk = $dst->$itemPk;
			$qs->save();
		}
		trace("******************in create mgr cloneQuestionnaire3", 3);
					
		return $dst;
	}
	
	
	
	/**
	 * Delete a questionnaire item and all its questions.
	 */
	function deleteQuestionnaire()
	{
		$item = $this->item;
		
		if(!$item->allowDelete())
		{
			return false;
		}
		$itemPk = $item->getPrimaryKey();	
		$obj = $this->getSortableObj();
		$pk = $obj->getPrimaryKey();
		
		$questions = $this->getQuestions();
		$qPk = $this->getQuestionKey();
		
		$tx = new DataTransaction();
		
		try
		{
			if(count($questions) > 0)
			{
				$obj->delete("WHERE {$itemPk}={$item->$itemPk}");
				foreach($questions as $question)
				{
					$question->joinTransaction($tx);
					$this->deleteOrphanQuestion($question);
				}
			}
			$item->joinTransaction($tx);
			$item->delete();		
			$tx->commit();
		}
		catch(Exception $e)
		{
			Fakoli::end($e->getMessage());
		}
		
		return true;
	}
	
	function logicalDeleteQuestionnaire()
	{
		if($this->item->hasField("deleted"))
		{
			$this->item->deleted = true;
			$this->item->filter = new InclusionFilter("deleted");
			$this->item->save();
		}	
	}
	
	
	
	/**
	 * Used when the question is linked through an xref table to the
	 * questionnaire/survey obj
	 * 
	 * @param $xref obj of xref class that links questions to questionnaire/survey
	 */
	function removeQuestionXref($xref_id)
	{
		$item = $this->item;
		$itemPk = $item->getPrimaryKey();
		$xref_class = $this->getXrefClass();
		
		try
		{
			$xref = new $xref_class($xref_id);
			$xrefPk = $xref->getPrimaryKey();
			
			if ($item->$itemPk != $xref->$itemPk)
			{
				throw new FakoliException("QuestionnaireManager removeQuestion Data Mismatch");
			}
			$xref->delete("WHERE $xrefPk={$xref_id}");
			$this->reNumberQuestions();
		}
		catch(Exception $e)
		{
			Fakoli::end($e->getMessage());
		}	
		
		$qPk = $this->getQuestionKey();
		$qClass = $this->getQuestionClass();
		$question = new $qClass($xref->$qPk);
		$this->deleteOrphanQuestion($question);
	}
	
	/**
	 * Used when the question is linked through an xref table to the
	 * questionnaire/survey obj
	 * 
	 * @param $question_id
	 */
	function removeQuestion($question_id)
	{
		$item = $this->item;
		$itemPk = $item->getPrimaryKey();
		$qClass = $this->getQuestionClass();	
		$question = new $qClass($question_id);
		$qPk = $question->getPrimaryKey();
		$xref = $this->getSortableObj();
		
		try
		{
			$xref->delete("WHERE {$itemPk}={$item->$itemPk} AND {$qPk}={$question->$qpK}");
			$this->reNumberQuestions();
		}
		catch(Exception $e)
		{
			Fakoli::end($e->getMessage());
		}
		
		$this->deleteOrphanQuestion($question);
	}
	
	/**
	 * Called from question form. On save question, save the
	 * question xref record.
	 * 
	 * @param $question
	 * @param $sort_order - has a value if adding multiple questions, called from 
	 * saveSelectedQuestions
	 */
	function saveQuestionXref($question, $sort_order = 0)
	{	
		$item = $this->item;
		$xref_class = $this->getXrefClass();
		$itemPk = $this->getPrimaryKey();
		$qPk = $question->getPrimaryKey();
				
		if(!$xref_class)
		{
			return;
		}
		
		$xref = new $xref_class();
			
		if($xref->exists("WHERE {$itemPk}={$item->$itemPk} AND {$qPk}={$question->$qPk}"))
		{
			return;
		}
	
		$xref->$itemPk = $item->$itemPk;
		$xref->sort_order = (!$sort_order) ? $this->getNextSortOrder() : $sort_order;
		$xref->$qPk = $question->$qPk;
		$xref->save();
	}
	
	/**
	 * If the question xref that was deleted was the only one linked to the
	 * question, then delete the question.
	 * 
	 * @param $question
	 */
	function deleteOrphanQuestion($question)
	{
		$xrefClass = $this->getXrefClass();
		$qPk = $this->getQuestionKey();
		
		if(!$xrefClass)
		{
			return;
		}
		
		$count = Query::create($xrefClass, "WHERE {$qPk}=:{$qPk}")
			->bind(":{$qPk}", $question->$qPk)
			->executeValue("COUNT(1)");
	
		if(count == 0)
		{
			$question->delete();
		}
	}
	
	/**
	 * Used when the question obj is linked directly to the
	 * questionnaire/survey item.
	 * 
	 * Called from delete_question action handler
	 * 
	 * @param $question_id
	 */
	function deleteQuestion($question_id)
	{
		$item = $this->item;
		$qClass = $this->getQuestionClass();
		$itemPk = $item->getPrimaryKey();
		$question = new $qClass($question_id);
		$qPk = $question->getPrimaryKey();
		$item_class = $this->item->prettifyClassName();
		$question_class = $question->prettifyClassName();
		
		if(!$question->allowDelete())
		{
			Fakoli::end("Delete not permitted for question id {$question->$qPk} {$question->question}");
		}
		
		try
		{
			if ($item->$itemPk != $question->$itemPk)
			{
				throw new FakoliException("QuestionnaireManager deleteQuestion Data Mismatch $item_class {$itemPk} {$item->$itemPk} does not match question $question_class {$itemPk} {$question->$itemPk} {$qPk} {$question->$qPk}");
			}
			
			$question->delete();
			$this->reNumberQuestions();
		}
		catch(Exception $e)
		{
			Fakoli::end($e->getMessage());
		}	
		
	}
	
	
	/**
	 * Reorder and renumber questions on save question list page.
	 * 
	 * The function expects that the sort order input field be labeled "question_{question_id}"
	 * as is the case if QuestionTableHelper formatSortOrder is used to create the field.
	 * 
	 * The function expects that the sort order field is contained in the Question class
	 * and is either named "sort_order" or "question_number".
	 */
	function reOrderQuestions()
	{
		global $_POST;

		$item = $this->item;
		$sortableClass = $this->getSortableClass();
		$sortableObj = new $sortableClass();
		$pk = $sortableObj->getPrimaryKey();
		$itemPk = $item->getPrimaryKey();
		$sort_field = $this->getSortField();
			
		// reorder
		foreach($_POST as $name => $sort_order)
		{
			if (!strncmp($name, "question_", 9))
			{
				$id = substr($name, 9);
				checkNumeric($id);
				checkNumeric($sort_order);
	
				$sortableObj = new $sortableClass($id);
				$sortableObj->filter = new InclusionFilter($pk, $sort_field);	
				$sortableObj->$itemPk = $item->$itemPk;
				$sortableObj->$pk = $id;
				$sortableObj->$sort_field = $sort_order;
				$sortableObj->save();
			}
		}
		
		$this->reNumberQuestions();
	}	
		
	
	/**
	 * Instantiates the xref class if set or question class if not
	 * to sort the items.
	 * 
	 * Retrieves the sortable objs through the function getSortableItems
	 * which expects that the mgr's item has the relation of pluralized
	 * xref class or pluralized question class
	 * 
	 */
	function reNumberQuestions()
	{
		$item = $this->item;
		$sortableClass = $this->getSortableClass();
		$sortableObj = new $sortableClass();
		$pk = $sortableObj->getPrimaryKey();
		$sort_field = $this->getSortField();
		
		$objs = $this->getSortableItems();
		$sort_order = 1;
		
		foreach($objs as $obj)
		{
			$obj->filter = new InclusionFilter($sort_field);
			$obj->$sort_field = $sort_order;
			$obj->save();
			$sort_order++;
		}		
	}
	
	
	/**
	 * Retrieve all the question sets for all surveys/questionnaires
	 * but exclude any questions already linked to the given
	 * item. 
	 */
	function getQuestionSets()
	{
		$itemPk = $this->item->getPrimaryKey();
		$qPk = $this->getQuestionKey();
		$xref = $this->getSortableObj();
		$xref_class = get_class($xref);
		
		if(!$xref)
		{
			return;
		}
		
		$query = GroupedQuery::create($xref_class, "", "{$itemPk}");
		if($this->item->$itemPk)
		{
			$query->constraints = "WHERE {$qPk} NOT IN (SELECT {$qPk} FROM {$xref->table} WHERE {$qPk} = :{$qPk})";
			$query->bind(":{$qPk}", $this->item->$itemPk);
		}
		
		return $query->execute();
	}
	
	/**
	 * Called from question select page for components that link questions
	 * through an xref
	 * 
	 * @param Array $items questionnaires/surveys user can select from. Components
	 * may have criteria to include/exclude certain records
	 *  
	 */
	function buildQuestionSelectForm($items)
	{
		$this->item->filter = new InclusionFilter();
		$form = new AutoForm($this->item);
		$itemPk = $this->getPrimaryKey();
		$title_field = $this->getTitleField();
		
		$questionSets = $this->getQuestionSets();
		
		$table = new GroupedDataListView($questionSets, "QuestionSets");
		$table->mode = "fixed";
		$table->selector();
		$table->column("Question", "{Question.question}", false)
	  		->column("Options", array(QuestionTableHelper, formatOptions), false)
	  		->column("Required", array(QuestionTableHelper, formatRequired), false, "text-align: center")
			;
		$table->emptyMessage = "There are no survey questions to select.";

		foreach(array_keys($questionSets) as $item_id)
		{
			if(!array_key_exists($item_id, $items)) continue;
	
			$item = $items[$item_id];
			$table->group($item->$title_field, $item_id);
		}

		$instructions = TextLookup::getText("question_select_instructions");
		$questionSelect = new DataListFieldRenderer($form, $table, "survey_question_select", "<h3>Select Survey Questions</h3>$instructions");
		$questionSelect->hideLabel = true;
		$form->submitLabel = "Select";
		$form->buttons_at_top = true;
		
		if(!count($questionSets))
		{
			$form->readOnlyForm = true;
		}

		$questionSelect->onPostProcess = array($this, saveSelectedQuestions);
		
		return $form;
	}
	
	/*
	 * Called by DataListFieldRenderer onPostProcess
	 * from the question select page.
	 * 
	 * Save questions selected
	 *
	 * It is possible for the user to select the same question
	 * if that question is part of 2 or more surveys.
	 */
	function saveSelectedQuestions($questionSelect, $field)
	{
		global $_POST;

		$xref_class = $this->getXrefClass();
		if(!$xref_class)
		{
			return;
		}
		
		$xref = new $xref_class();
		$xrefPk = $xref->getPrimaryKey();
		$questionClass = $this->getQuestionClass();
		$qPk = $this->getQuestionKey();
	
		$questionIds = $_POST[$xrefPk];

		if(isset($questionIds) AND count($questionIds) > 0)
		{
			$list = implode(",", array_values($questionIds));
			// Given the set of xref ids selected, retrieve the questions from the question table
			$questions = Query::create($questionClass, "WHERE {$qPk} IN (SELECT {$qPk} FROM {$xref->table} WHERE {$xrefPk} IN ($list))")
				->execute();

			if(count($questions) > 0)
			{
				$sort_order = $this->getNextSortOrder();

				foreach($questions as $question)
				{
					$this->saveQuestionXref($question, $sort_order);
				}
			}
		}
		return true;
	}
	
	/**
	 * From the question list page, set the table to allow draggable reorder.
	 * 
	 * @param $table
	 */
	function enableDragReorder(&$table)
	{
		$component_name = $this->getComponentName();
		$reorder_handler = $this->getReorderHandler();
		$itemPk = $this->getPrimaryKey();
		
		$table->enableDragReorder("/action/{$component_name}/{$reorder_handler}?{$itemPk}={$this->item->$itemPk}");
		$table->dragText = "<span style='font-size: 10px'>Click and drag to change the order of the questions</span>";	
	}
	
	/**
	 * Called from reorder_questions action handler which
	 * responds to draggable reordering in data list view.
	 * 
	 * @param String $orderClass - name of the DataItem class that stores the
	 * question order (e.g., Question, or SurveyQuestionXrefQuestion)
	 * This class must have a field named "sort_order" or "question_number"
	 * and have a field that matches the item's pk field (e.g., questionnaire_id or
	 * survey_id)
	 */
	function saveDraggableQuestionOrder()
	{
		$item = $this->item;
		$itemPk = $item->getPrimaryKey();
		$sortableClass = $this->getSortableClass();
		$sortableObj = new $sortableClass();
		$sort_field = $this->getSortField();
			
		$pk = $sortableObj->getPrimaryKey();
		$tx = new DataTransaction();
		
		try
		{
			foreach($_GET[$pk] as $id => $sort_order)
			{
				checkNumeric($id);
				checkNumeric($sort_order);
		
				$sortableObj = new $sortableClass();
				$sortableObj->joinTransaction($tx);
			
				$sortableObj->load($id);
			
				if ($item->$itemPk != $sortableObj->$itemPk)
				{
					throw new FakoliException("Data Mismatch");
				}
				
				$sortableObj->filter = new InclusionFilter($pk, $sort_field);
				$sortableObj->$sort_field = $sort_order;
				$sortableObj->save();
			}
		
			$tx->commit();
		}
		catch(Exception $e)
		{
			$tx->rollback();
			Fakoli::end($e->getMessage());
		}					
	}
	
	/**
	 * Get the next sort order for a new question.
	 * 
	 * If the questions are linked to the questionnaire directly, then
	 * query the Question class.
	 * 
	 * If the questions are linked through an xref, then query the
	 * xref class.
	 */
	function getNextSortOrder()
	{
		$item = $this->item;
		$sortableClass = $this->getSortableClass();
		$itemPk = $item->getPrimaryKey();
		$sort_field = $this->getSortField();
			
		return Query::create($sortableClass, "WHERE {$itemPk}=:{$itemPk}")
			->bind(":{$itemPk}", $item->$itemPk)
			->executeValue("MAX($sort_field)") + 1;
	}
	

	/**
	 * Define the set of valid actions for the dashboard or any other
	 * page that provides buttons/links to perform an action on a 
	 * questionnaire/survey.
	 * 
	 * Implementing components may have additional logic to unset from
	 * available actions those that don't apply to a particular
	 * questionnaire/survey instance.
	 */
	function getActions()
	{
		$actions = $this->item->getAvailableActions();

		return $actions;
	}
	
	/**
	 * Instantiate the QuestionnaireCreateManager js class.
	 * 
	 * Requires the following handlers to exist in the questionnaire component.
	 * The item class is used for the error tag for dialog results js function.
	 * 
	 * Default question handler names:
	 * question_delete - used when no xref
	 * question_remove - used when questions linked through xref
	 * reorder_questions
	 * 
	 */
	function writeScript()
	{
		$item = $this->item;
		$qPk = $this->getQuestionKey();
		$itemPk = $this->item->getPrimaryKey(); 
		$item_id = $this->item->$itemPk;
		$component_name = $this->getComponentName();
		$question_delete_handler = $this->getQuestionDeleteHandler();
		$question_remove_handler = $this->getQuestionRemoveHandler();
		$question_list_identifier = $this->getQuestionListIdentifier();

		$xrefPk = "";
		$xref = $this->getSortableObj();
		if($xref)
		{
			$xrefPk = $xref->getPrimaryKey();
		}
		
		ob_start();
?>
<script type="text/javascript" src="/components/questionnaire/js/questionnaire_create.js"></script>
<script type="text/javascript">
var questionnaireMgr;


window.addEvent('domready', function()
{
	questionnaireMgr = new QuestionnaireCreateManager(
			'<?php echo $qPk ?>',
			 '<?php echo $itemPk ?>',
			<?php echo $item_id ?>,
			'<?php echo $xrefPk ?>',
			'<?php echo $component_name ?>',
			'<?php echo $question_delete_handler ?>',
			'<?php echo $question_remove_handler ?>',
			'<?php echo $question_list_identifier ?>'
			);
});

</script>
<?
		$script .= ob_get_contents();
		ob_end_clean();
		
		return $script;
	}	
	
}

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
	
	function AbstractQuestionnaireResponseManager($item)
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

	function getResponseIntroIdentifier()
	{
		$class = codify(strtolower($this->item->prettifyClassName()));
		return "{$class}_response_intro";
	}
	
	function getResponseFormIdentifier()
	{
		$class = codify(strtolower($this->item->prettifyClassName()));
		return "{$class}_response_form";
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
			return $item->Questionnaire();
		}
		elseif($response->hasRelation(Survey))
		{
			return $item->Survey();
		}
		else
		{
			Fakoli::end("No relation from response to questionnaire/survey");
		}
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
		$questionnaire = $this->item->getQuestionnaire();
		$itemPk = $questionnaire->getPrimaryKey(); 
		$item_id = $questionnaire->$itemPk;
		$responsePk = $this->item->getPrimaryKey(); 
		$component_name = $this->getComponentName();
		$exclude_handler = $item->getExcludeHandler();
		$response_intro_identifier = $this->getResponseIntroIdentifier();
		$response_form_identifier = $this->getResponseFormIdentifier();
		
		ob_start();
?>
<script type="text/javascript" src="/components/questionnaire/js/questionnaire_response.js"></script>
<script type="text/javascript">
var questionnaireMgr;

window.addEvent('domready', function()
{
	questionnaireMgr = new QuestionnaireResponseManager('<?php echo $itemPk ?>', 
		'<?php echo $item_id ?>', '<?php echo $responsePk ?>',  
		'<?php echo $component_name ?>',
		'<?php echo $exclude_handler ?>',
		'<?php echo $response_intro_identifier ?>',
		'<?php echo $response_form_identifier ?>'
		);
});

</script>
<?
		$script .= ob_get_contents();
		ob_end_clean();
		
		return $script;
	}	
	
}


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
	 */
	abstract function getResponseManager($response);

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
		$form->ajaxSubmit("function(result) {new questionnaireMgr.questionNamesFormResult(result);}", "function() {\$('{$form->id}_error').set('text','Failed to communicate with server'); }");
		
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
		'<?php echo $response_view_title ?>');
});

</script>
<?
		$script .= ob_get_contents();
		ob_end_clean();
		
		return $script;
	}	
	
}

/**
 * Provides static function support for Questionnaire Manager
 * methods that are shared among types
 * QuestionnaireCreateManager extends AbstractQuestionnaireCreateManager
 * QuestionnaireResponseManager extends AbstractQuestionnaireResponseManager
 * QuestionnaireResultsManager extends AbstractQuestionnaireResultsManager
 * 
 * For example, all three need the function getQuestionNumber and getQuestions, 
 * so they should refer their methods to this static method to avoid duplicate 
 * code.
 *
 */
class QuestionnaireHelper
{
	function QuestionnaireHelper()
	{
		
	}
	
	static function getQuestionNumber($item, $questions, $question)
	{
		if(count($questions) > 0)
		{
			$idx = 1;
			foreach($questions as $q)
			{
				$renderer = QuestionField::create($item, $q);
				
				if (!$renderer->skipNumbering)
				{
					$idx++;
				}
				
				if ($question->question_id == $q->question_id) return $idx;
			}
		}
		return "";		
	}
	
	/**
	 * Given an array of question objects, set the question_number
	 * field or pseudo field so that Question Renderer won't need to
	 * call getQuestionNumber for each question.
	 * 
	 * @param Array $questions - array of objects of a Question DataItem class
	 */
	static function setQuestionNumbers($item, $questions)
	{
		if(count($questions) == 0) return $question;
		
		$idx = 1;
		foreach($questions as $question)
		{
			$renderer = QuestionField::create($item, $question);
			
			if (!$renderer->skipNumbering)
			{
				$question->question_number = $idx;
				$idx++;
			}
		}
		return $questions;
	}	


	/**
	 * On the question names dialog, format the input field for editing
	 * the question names.
	 * 
	 * @param obj $question
	 */
	static function formatQuestionNameInputField($question)
	{
		if(!$question->question_name)
			$question->question_name = substr($question->question, 0, 79);

		$pk = $question->getPrimaryKey();
		return $question->format("<input type='text' name=\"question_{$question->$pk}\"
		value=\"{question_name}\" size=\"40\"/>");	
	}	
		
} // end QuestionnaireHelper

?>