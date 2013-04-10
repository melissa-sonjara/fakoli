<?php
Fakoli::using("questionnaire", "settings");
Fakoli::usingFile("/cms/components/questionnaire/abstract_questionnaire_manager.inc");

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
 * include_in_results - default true - if have this field, must have function excludeResponse
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

define("questionnaire_sent", "1");
define("questionnaire_send_failed", "2");
define("questionnaire_opened", "3");
define("questionnaire_no_additional", "4");
define("questionnaire_reminder_sent", "5");
define("questionnaire_reminder_failed", "6");
define("questionnaire_test_sent", "7");
define("questionnaire_test_failed", "8");

abstract class AbstractQuestionnaireSendManager extends AbstractQuestionnaireManager
{
	var $item;
	var $recipients;					///< Comma delimited string of all email recipients
	var $responses = array();			///< Array of objs of response DataItem class 
	var $excludes = array();			///< Array of emails to be set to excluded from results
	var $addl_recipients = array();		///< Array of additional recipients after duplicates have been removed

	function AbstractQuestionnaireSendManager($item)
	{
		$this->item = $item;

		$identifier = $_REQUEST["identifier"];
		$sendId = $this->getSendPageIdentifier();

		if($sendId == $_REQUEST["identifier"])
		{
			$this->validateSend();
		}
	}

	abstract function getComponentName();
	
	/**
	 * Get an instance of the create manager
	 * so that we don't duplicate specifications
	 * here.
	 */
	abstract function getCreateManager();
	
	/**
	 * The DataItem class that stores the responses
	 * (e.g., QuestionnaireResponse)
	 */
	abstract function getResponseClass();
	
	/**
	 * Get an instance of the results manager 
	 * so that we don't duplicate its code.
	 */
	abstract function getResultsManager();
	
	
	/**
	 * Display name for the questionnaire/survey data item
	 * class to be displayed in info text and buttons.
	 */
	function getClassName()
	{
		return $this->item->prettifyClassName();
	}


	/**
	 * Action handler dialog to enter tester emails.
	 * 
	 * @return string
	 */
	function getSendTestEmailDialog()
	{
		return "send_test_email_dialog";
	}

	/**
	 * Action handler to send test email to user.
	 * 
	 * @return string
	 */
	function getSendTestEmailHandler()
	{
		return "send_test_email";
	}

	/**
	 * Action handler dialog to send reminders
	 * to nonresponders.
	 * 
	 * @return string
	 */
	function getReminderDialog()
	{
		return "reminder_dialog";
	}
	
	/**
	 * Action handler to send emails to additional recipients.
	 * 
	 * @return string
	 */
	function getSendAdditionalHandler()
	{
		return "send_additional_emails";
	}

	/**
	 * Action handler dialog that displays the token
	 * or name values of recipients or additional
	 * recipients.
	 * 
	 * @return string
	 */
	function getRecipientsDialog()
	{
		return "recipients_dialog";
	}

	/**
	 * Action handler to open the questionnaire/survey
	 * e.g., open_survey
	 * 
	 * @return string
	 */
	function getOpenHandler()
	{
		$class = codify(strtolower($this->getClassName()));
		return "open_{$class}";
	}

	/**
	 * Action handler to close the questionnaire/survey
	 * e.g., close_survey
	 * 
	 * @return string
	 */
	function getCloseHandler()
	{
		$class = codify(strtolower($this->getClassName()));
		return "close_{$class}";
	}

	/**
	 * Action handler dialog to allow user to select
	 * an email message from among existing questionnaires/surveys.
	 * 
	 * @return string
	 */
	function getMessageSelectDialog()
	{
		return "message_select_dialog";
	}

	/**
	 * Identifier of the preview/manage page.
	 * 
	 * @return string
	 */
	function getSendPageIdentifier()
	{
		$class = codify(strtolower($this->getClassName()));
		return "{$class}_preview";
	}

	/**
	 * Identifier of the confirmation page.
	 * 
	 * @return string
	 */
	function getConfirmationPage()
	{
		$class = codify(strtolower($this->getClassName()));
		return "{$class}_confirmation";
	}

	/**
	 * Identifier of the email form page.
	 * 
	 * @return string
	 */
	function getEmailForm()
	{
		$class = codify(strtolower($this->getClassName()));
		return "{$class}_email";
	}

	/**
	 * Get the identifier of the question form 
	 * from the create manager.
	 * 
	 * @return string
	 */
	function getQuestionForm()
	{
		$mgr = $this->getCreateManager();
		return $mgr->getQuestionForm();
	}

	/**
	 * Get the identifier of the questionnaire form
	 * (first tab) from the create manager.
	 * 
	 * @return string
	 */
	function getQuestionnaireFormIdentifier()
	{
		$mgr = $this->getCreateManager();
		return $mgr->getQuestionnaireFormIdentifier();
	}

	/**
	 * Get the identifier of the question list
	 * page from the create manager.
	 * 
	 * @return string
	 */
	function getQuestionListIdentifier()
	{
		$mgr = $this->getCreateManager();
		return $mgr->getQuestionListIdentifier();
	}

	/**
	 * Get the default value of the sender email for the
	 * email form.
	 * 
	 * @return string
	 */
	function getSenderEmail()
	{
		$item = clone $this->item;
		$item->filter = null;
		$component_name = $this->getComponentName();
		
		global $user;

		if($item->hasField("sender_email") && $item->sender_email)
		{
			return $item->sender_email;
		}

		$email = Settings::getValue($component_name, "sender_email");

		if(!$email)
		{
			$email = $user->email;
		}

		return $email;
	}

	/**
	 * On the survey email form, set the recipients, cc_recipients,
	 * and additional recipients fields
	 *
	 */
	function setOneRecipientField(&$form, $field, $label = "", $columns = 80)
	{
		$renderer = new EmailListFieldRenderer($form, $field, $label);
		$renderer->removeDuplicates = true;
		$renderer->columns = $columns;
	}


	/**
	 * Get the recipients from the following fields, if in the
	 * field list for the item:
	 * recipients (required)
	 * cc_recipients (optional)
	 */
	function getAllRecipients()
	{
		$item = $this->item;
		$item->filter = null;
		
		$recipients = explode(",", $item->recipients);

		if($item->hasField("sender_email") && $item->sender_email)
		{
			if(array_search($item->sender_email, $recipients !== FALSE))
			{
				$this->excludes[] = $item->sender_email;
			}
		}

		if($item->hasField("user_id") && $item->user_id)
		{
			$user = $item->Author();
			if(array_search($user->email, $recipients) !== FALSE)
			{
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
	 * @param Array $recipients
	 */
	function generateResponses($recipients)
	{
		$item = $this->item;
		$responseClass = $this->getResponseClass();
		$itemPk = $item->getPrimaryKey();
		$responses = array();

		$indexedResponses = IndexedQuery::create($responseClass, "WHERE {$itemPk}=:{$itemPk}", "email")
			->bind(":{$itemPk}", $item->$itemPk)
			->execute();

		foreach($recipients as $recipient)
		{
			if(!array_key_exists($recipient, $indexedResponses))
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
	* 
	* @return obj instance of response class
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
	 * Create a sample response for the Survey Preview & Send page.
	 * If the survey has a user author, then check if we already have a SurveyResponse
	 * record for that user's email. If not, create a sample response for the
	 * author. If no author, then create a sample response for the logged in user.
	 * 
	 * @return obj instance of response class
	 */
	function getSampleResponse()
	{
		global $user;

		$author = $this->item->Author();
		$email = ($author) ? $author->email  : $user->email;

		$response = $this->findResponseByEmail($email);

		if(!$response)
		{
			$response = $this->generateToken($email);
		}

		if(!$response->isTester())
		{
			$response->excludeResponse();
		}

		return $response;
	}

	/**
	 * Given an email address, find a response record with that email, if
	 * it exists.
	 * 
	 * @param String $email
	 * @return null or obj instance of response class
	 */
	function findResponseByEmail($email)
	{
		$response_class = $this->getResponseClass();
		$pk = $this->getPrimaryKey();

		$responses = Query::create($response_class, "WHERE {$pk}=:{$pk} AND email=:email")
			->bind(":{$pk}", $this->item->$pk, ":email", $email)
			->execute();
			
		return (count($responses) > 0) ? $responses[0] : null;
	}


	/**
	 * Set cc recipient, sender, and author to be excluded from results. 
	 * 
	 * calls function excludeResponse in response class DataItem
	 */
	function setExcluded()
	{
		$item = $this->item;

		$indexedResponses = reindexList($this->responses, "email");

		if(!count($this->excludes))
		{
			return;
		}

		foreach($this->excludes as $excludeEmail)
		{
			if(!array_key_exists($excludeEmail, $indexedResponses)) continue;
				
			$response = $indexedResponses[$excludeEmail];
			
			// In case user puts same email in recipients and additional or cc recipients
			if(is_array($response))
			{
				$response = $response[0];
			}
				
			$response->excludeResponse();
		}
	}
	
	/**
	 * Custom save handler from preview/manage page
	 * form.
	 * 
	 * @param obj $form
	 */
	function sendRequests($form)
	{
		$rtn = $this->sendToRecipients();
		if($rtn == questionnaire_sent)
		{
			$this->item->setStatus("open");
		}
		
		return $rtn;
	}

	function sendToRecipients()
	{
		if($this->item->additional_recipients)
		{
			return $this->sendToAdditional();
		}
		
		$recipients = $this->getAllRecipients();
		
		if(!$recipients)
		{
			if($this->allowAnonymous())
			{
				$this->item->setStatus("open");
				return questionnaire_opened;
			}
			return;
		}

		$this->responses = $this->generateResponses($recipients);

		$this->setExcluded();

		if(!count($this->responses))
		{
			return;
		}

		$rtn = $this->sendEmails();

		return $rtn;
	}
	
	function allowAnonymous()
	{
		$item = clone $this->item;
		$item->filter = null;
		
		return ($item->hasField("allow_anonymous_responses") && $item->allow_anonymous_responses) ? true : false;
	}

	function sendActions()
	{
		$class = $this->getClassName();
		return array (
				"send_additional"	=>	"Send Additional Emails", // only on preview/manage page
				"send_test"			=>	"Send Test Email To...",
				"send"				=>	"Preview &amp; Send",
				"close"				=>	"Close $class",
				"reopen"			=>	"ReOpen $class",
				"send_reminders"	=>	"Send Reminders",
		);
	}

	/**
	 * Remove actions that are not applicable given the status of
	 * the survey/questionnaire.
	 *
	 */
	function getSendActions()
	{
		$item = clone $this->item;
		$item->filter = null;
		
		// No send actions are valid when the survey is not valid
		if(!$this->isValid()) return;

		$sendActions = $this->sendActions();

		if(!is_array($sendActions) || !count($sendActions)) 
		{
			return null;
		}
			
		/**
		 * From dashboard, show "send" that redirects
		 * to preview/manage page if not send or if
		 * open and there are additional recipients.
		 */
		$send_identifier = $this->getSendPageIdentifier();
		$identifier = $_REQUEST["identifier"];
		if(($item->isSent() && !$this->hasAdditional()) || $identifier == $send_identifier || $item->isClosed())
		{
			unset($sendActions["send"]);
		}
		
		/**
		 * From preview/manage page, show send additional,
		 * if applicable
		 */
		if(!$this->hasAdditional() || $identifier != $send_identifier)
		{
			unset($sendActions["send_additional"]);
		}
		
		if(!$item->isOpen())
		{
			unset($sendActions["close"]);
		}

		if(!$item->isClosed())
		{
			unset($sendActions["reopen"]);
		}

		$nonResponseCount = count($this->getNonResponsives());	
		if($nonResponseCount == 0 || $item->isClosed())
		{
			unset($sendActions["send_reminders"]);
		}

		return $sendActions;
	}
	
	function hasAdditional()
	{
		$item = clone $this->item;
		$item->filter = null;
		
		if($item->hasField("additional_recipients") && $item->additional_recipients)
		{
			return true;
		}
		
		return false;
	}

	/**
	 * Send an open survey to additional recipients.
	 * 
	 * If none of the additional recipients are not also in
	 * the original recipient list, remove all and save.
	 */
	function sendToAdditional()
	{
		$item = $this->item;
		$item->filter = null;
		
		if(!$this->hasAdditional())
		{
			return;
		}
		
		$addl_recipients = explode(",", $item->additional_recipients);
		$addl_recipients = array_unique($addl_recipients);
		
		// array_diff not providing expected result
		$new_recipients = array();
		foreach($addl_recipients as $addl_recipient)
		{
			if(!preg_match("/$addl_recipient/", $item->recipients))
			{
				$new_recipients[] = $addl_recipient;
			}
		}
		
		$addl_recipients = $new_recipients;

		if(!count($addl_recipients))
		{
			$item->additional_recipients = '';
			$item->filter = new InclusionFilter("additional_recipients");
			$item->save();
			return questionnaire_no_additional;
		}

		$this->excludes = $addl_recipients;
		$this->addl_recipients = $addl_recipients;

		$this->responses = $this->generateResponses($addl_recipients);

		$this->setExcluded();

		if(!count($this->responses))
		{
			return;
		}

		$rtn = $this->sendEmails();

		$this->moveAdditionalToRecipientList();

		return ($rtn) ? questionnaire_sent : questionnaire_send_failed;
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
			$rtn = $this->sendOneEmail($response, $subject, $message, $sender_email);
		}

		$this->moveAdditionalToRecipientList();

		return $rtn;
	}

	function getSubject()
	{
		$item = clone $this->item;
		$item->filter = null;
		$component_name = $this->getComponentName();
		
		if($item && $item->hasField("subject") && $item->subject)
		{
			return $item->subject;
		}

		return Settings::getValue($component_name, "email_subject");
	}

	function getMessage()
	{
		$component_name = $this->getComponentName();
		
		$footer = Settings::getValue($component_name, "message_footer");
		$message = $this->item->message;

		if($footer)
		{
			$message .=  "\r\n\r\n{$footer}";
		}

		return $message;
	}

	function sendOneEmail($response, $subject, $message, $sender_email)
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
		$item->filter = null;

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

		$this->responses = $this->generateResponses($recipients);

		$this->excludes = $recipients;

		$this->setExcluded();

		if(!count($this->responses))
		{
			return;
		}

		return $this->sendEmails();
	}

	/**
	 * Get the list of recipients who have not responded.
	 */
	function getNonResponsives()
	{
		return $this->item->getNonResponders();
	}

	/**
	 * Called from survey reminder dialog.
	 *
	 * The var $recipients is set when buildReminderForm is called from the
	 * dialog.
	 *
	 */
	function sendReminderEmails()
	{
		$recipients = explode(",", $this->recipients);

		if(!count($recipients)) return;

		$this->responses = $this->generateResponses($recipients);

		if(!count($this->responses)) return;
	
		return $this->sendEmails();
	}

	/**
	 * Called from sent_test_email_dialog
	 */
	function buildSendTestEmailForm($dashboard = 0)
	{
		global $user;

		$item = $this->item;
		$component_name = $this->getComponentName();
		$dialog = $this->getSendTestEmailDialog();
		$itemPk = $this->getPrimaryKey();

		$email = new EmailTemplate();
		$email->filter = new InclusionFilter("recipients");
		$email->recipients = $user->email;

		$form = new AutoForm($email, "POST", "/action/{$component_name}/{$dialog}?{$itemPk}={$item->$itemPk}&dashboard=$dashboard", "SendTestEmail_form");
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
	function buildReminderForm($dashboard = 0)
	{
		$item = $this->item;
		$component_name = $this->getComponentName();
		$handler = $this->getReminderDialog();
		$pk = $this->getPrimaryKey();
		$id = $this->item->$pk;

		$mgr = $this->getResultsManager();
		$nonResponsives = $item->getNonResponders();
		// displayed in modal dialog
		$form->data->set("nonresponsive_count", count($nonResponsives));
		$item->recipients = formatItems($nonResponsives, "{token}", ", ");
		// Set the recipient list for use when form is submitted
		$this->recipients = formatItems($nonResponsives, "{email}", ",");

		$item->filter = new InclusionFilter("title", "recipients", "sender_email", "message", "subject");
		$form = new AutoForm($item, "POST", "/action/{$component_name}/{$handler}?{$pk}={$id}&dashboard=$dashboard", "Reminder_form");
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
	 * On Survey Response pages, show a "For Internal Use Only" type of
	 * grey box to logged in users who are permitted to view the survey
	 * and to those who have a test survey response token.
	 *
	 * Allow survey users to send themself a test email with a token.
	 *
	 * If either type of user already has a token, they may reset/reinitialize it.
	 *
	 * If the survey is closed, put the link to the survey inside the grey
	 * box and let them know they can override the closed status because
	 * they are test users.
	 *
	 * @param obj $survey
	 */
	function getTesterIntro($response)
	{
		global $user;
		$item = $this->item;
		$pk = $this->getPrimaryKey();
		$item_id = $item->$pk;
		$response_class = $this->getResponseClass();

		$out = "<div class='questionnaire_tester_block'>\n";
		$out .= "<p id='action_result' class='questionnaire_action_result' style='display: none'></p>\n";

		if(!$response && $user)
		{
			$response = $this->findResponseByEmail($user->email);
		}
		
		if($response)
		{
			$editLink = $response->getEditUrl();
		}
	
		$out .= "<p>";
		if($user && $item->isAuthor())
		{
			$out .= "Since you have access privileges to this survey, ";
		}
		else if($response)
		{
			$out .= "Since you are a test user, ";
		}

		if(($user || $response) && $item->isClosed())
		{
			$out .= "you can override the closed survey status to try out this survey. ";
			$out .= "</br></br><a href=\"#\" onclick=\"questionnaireMgr.showResponseTokenDialog();\">{$item->title}</a>\n";
		}
		else
		{
			$out .= "you can access this survey using a test access token.\n";
		}

		$out .= "</p>\n";
		
		$out .= "<p>";
		if($response && $response->isSubmitted())
		{
			$out .= $response->format("Your survey response token <b>{token}</b> has been submitted. You can reset the token to start over.");
		}
		else if($response)
		{
			$out .= $response->format("You can use the link above to enter your survey token <b>{token}</b> or use this direct link $editLink.");
		}
		elseif($user && $item->isAuthor())
		{
			$out .= "If you would like to receive a survey token, send a test survey request email using one of the buttons below.";
		}
		
		if(!$response || ($response && $response->isTester()))
		{
			$out .= " Your responses will be excluded from the survey results. ";
		}
		else if($user && $response && !$response->isTester())
		{
			$out .= " Your response token is currently set to be included in the results. We recommend you set your response to be <a href='#' onclick=\"questionnaireMgr.excludeResponse({$response->response_id}); return false\">excluded</a> from results tabulation. ";
		}
		$out .= "</p>";
		
		$out .= "<div class='button_row' style='text-align: middle;'>\n";
		if(!$response && $user && $item->isAuthor())
		{
			$out .= "<a class='button' href='#' onclick=\"questionnaireSendMgr.sendTestEmail(); return false;\">Send Test Email to {$user->email}</a>\n";
		}
		else if($response && $response->isSubmitted())
		{
			$out .= $response->format("<a class='button' href='#' onclick=\"questionnaireMgr.resetToken({response_id}, 1); return false\">Reset Token</a>\n");
		}

		$out .= "</div>\n";
		$out .= "</div>\n";

		return $out;
	}

	/**
	 * For the survey_preview page, set the information message heading to
	 * match the survey send status.
	 */
	function getSendPageHeading($tabs = null)
	{
		$item = $this->item;
		$out = "";

		$out = "<div id='questionnaire_heading'>";
		/**
		 * Only show next page link if survey has been sent
		 */
		if($tabs && $item->isSent())
		{
			$nextPage = $tabs->getNextPage();
			$out .= "<button id='next_page_button' class='button' onclick=\"go('$nextPage')\">Next Page &raquo;</button>\n";
		}

		if (!$item->isSent())
		{
			$out .= "<h3>Survey Preview &amp; Send</h3>\n";
		}
		else
		{
			$out .= "<h3>Manage Survey</h3>\n";
		}

		$out .= $this->getStatusMessage();
		$out .= "</div>";

		if($item->isSent())
		{
			$out .= "<h4>Responses Received</h4>\n";
			$out .= $this->getProgressBar();
		}
		
		return $out;
	}
	
	/**
	 * Called by survey results/responses page. If survey is open and there are
	 * nonresponders, draw button to send remidners.
	 */
	function getResultsPageButtons()
	{
		$item = clone $this->item;
		$item->filter = null;
		
		$mgr = $this->getResultsManager();		
		$recipientCount = $mgr->getRecipientCount();
		$responseCount = $mgr->getResponseCount();
		
		$buttons = array();
		if($item->isOpen() AND $recipientCount > 0 AND $responseCount < $recipientCount)
		{
			$buttons[] = "<a class=\"button\" href='#' onclick=\"questionnaireSendMgr.showReminderDialog(); return false;\">Send Survey Reminders</a>&nbsp;&nbsp;\n";
		}
		
		if ($this->allowAnonymous())
		{
			$buttons[] = $item->format("<a target='_blank' href='anonymous_response?{$pk}={{$pk}}' class='button'>Enter an Anonymous Survey Response</a>\n");
		}
		
		if(count($buttons))
		{
			$out = "<p>" . implode("&nbsp;&nbsp", $buttons) . "</p>\n";
				
		}
		
		return $out;
	}
	
	function getProgressBar()
	{
		$mgr = $this->getResultsManager();
		return $mgr->getProgressBar();
	}

	function formatDefaultConfirmationMessage()
	{
		$sitename = Settings::getValue("settings", "sitename");

		return "<p>Thank you for submitting your survey.</p><p>Continue to the <a href='/'>{$sitename} home page.</a></p>";
	}


	function getStatusMessage()
	{
		$item = clone $this->item;
		$item->filter = null;
		
		$class = strtolower($this->getClassName());

		if (!$item->isSent())
		{
			if(!$this->allowAnonymous())
			{
				$out = "<p>This survey has not yet been sent.</p>";
			}
			else
			{
				$out = "<p>Since this $class allows anonymous responses, the recipient field is not required. However, if you specify one or more recipients, you must provide an email message.</p>\n";
			}
		}
		else
		{
			$out .= $item->format("<p>This survey was opened on {start_date:F d, Y}");
			if($item->isClosed())
			{
				$out .= $item->format(" and closed on {end_date:F d, Y}");
			}
			$out .= ".</p>";
		}

		return $out;
	}

	function getEmailPageHeading()
	{
		$item = $this->item;
		$out = "<div id='questionnaire_heading'>";
		$out .= "<p>";
		$out .= $this->getStatusMessage();

		if($item->isAuthor() && $item->isClosed())
		{
			$out .= " If you wish to send additional survey requests, you must first
			<a href='#' onclick=\"questionnaireSendMgr.openToRespondents(); return false;\">reopen</a>
			the survey.";
		}

		$out .= "</p>\n";

		return $out;
	}

	function getCCRecipientsAnnotation()
	{
		if($this->item->isClosed()) return "";
		
		global $user;

		return "CC Recipients will receive a copy of the email with a valid survey response token but their responses
		will not be counted in the results. (separate email addresses with a comma ',')</br>
		<div class='button_row'>\n<a class='button' onclick=\"questionnaireSendMgr.addEmailToCCRecipients('{$user->email}'); return false;\">Add Me To CC Recipients</a>&nbsp;&nbsp;
		<a class='button' onclick=\"questionnaireSendMgr.addEmailToCCRecipients('sender'); return false;\">Add Sender Email to CC Recipients</a>\n</div>\n";
		}

	function getMessageAnnotation()
	{
		if($this->item->isClosed()) return "";
		
		return "Select from a set of boilerplate messages which you can then customize for this survey.<br>
		<div class='button_row'>\n<a href='#' class='button' onclick=\"questionnaireSendMgr.showMessageSelectDialog(); return false;\">Select a Message</span></a>&nbsp&nbsp;
		<a href='#' class='button' onclick=\"questionnaireSendMgr.showAdvancedFeaturesDialog(); return false\">Advanced Features</a>\n</div>\n";
	}

	/*
	 * Draw a:link button options on Preview/Manage Survey tab
	*
	* public view
	* close/reopen
	* send test
	* send reminders
	*/
	function getManageLinks()
	{
		$item = clone $this->item;
		$item->filter = null;
		$class = $this->getClassName();
		
		$actions = $this->getSendActions();
		
		if(count($actions) > 0)
		{
			foreach($actions as $name => $label)
			{
				$buttons[] = "<a class='button' href=\"#\" onclick=\"questionnaireSendMgr.handleSendAction('{$name}'); return false;\">{$label}</a>";
			}
		}
		
		$out = "<p>" . implode("&nbsp;&nbsp;", $buttons) . "</p>\n";
		return $out;
	}

	/**
	* Validate that the survey is completed enough to be
	* sent:
	*
	* Must have message
	* If not allow anonymous, must have at least one recipient
	*/


	/**
	* Determine if the survey is valid. If not valid, do not
	* present options such as send survey and open survey.
	*
	* @return boolean
	*/
	function isValid()
	{
		$this->validation_msg = "";
		$mgr = $this->getCreateManager();

		$msg = $this->validateSend();

		if($msg) return false;

		$msg = $mgr->validateQuestionnaire();
		
		if($msg) return false;

		return true;
	}
	
	/**
	 * On Survey Preview/Send page, determine how the submit label should read
	 * based on whether anonymous responses, recipients, or cc recipients.
	 */
	function getSendSubmitLabel()
	{
		$item = clone $this->item;
		$item->filter = null;		
		$class = $this->getClassName();

		if($item->recipients || ($item->hasField("cc_recipients") && $item->cc_recipients))
		{
			$label = "Send $class Emails";
		}
		else
		{
			$label = "Open to Anonymous Respondents";
		}
		
		return $label;
	}

	function validate()
	{
		$this->validation_msg = "";
		$mgr = $this->getCreateManager();

		$msg = $mgr->validateQuestionnaire();
		if($msg) $msg .= "<br>";
		$msg .= $this->validateSend();
		$this->validation_msg = $msg;
	}

	function writeSendValidationMsg()
	{
		$msg = $this->validateSend();
		echo "<div id='warning'>{$msg}</div>\n";
	}

	function writeQuestionnaireValidationMsg()
	{
		$mgr = $this->getCreateManager();
		$mgr->writeQuestionnaireValidationMsg();
	}

	function validateSend()
	{
		$item = clone $this->item;
		$item->filter = null;
		
		$pk = $this->getPrimaryKey();
		$id = $item->$pk;
		$email_form = $this->getEmailForm();
		$msg = "";

		if(!$item->recipients && !$this->allowAnonymous())
		{
			if($msg) $msg .= "<br>";
			$msg .= "You must enter at least one recipient email address.";
		}
		
		$recipientFields[] = "recipients";
		$this->validateRecipients($msg, $item->recipients);
		
		if($item->hasField("cc_recipients"))
		{
			$this->validateRecipients($msg, $item->cc_recipients);
			$recipientFields[] = "recipients";
		}
		
		if($item->hasField("additional_recipients"))
		{
			$this->validateRecipients($msg, $item->cc_recipients);
			$recipientFields[] = "recipients";
		}
				
		$empty = true;
		foreach($recipientsFields as $field)
		{
			if($item->$field)
			{
				$empty = false;
			}
		}
		
		if(!$item->message && (!$this->allowAnonymous() || !$empty))
		{
			if($msg) $msg .= "<br>";
			$msg .= "You must enter a message for the email.";
		}
		
		return $msg;
	}
	
	function validateRecipients(&$msg, $values)
	{
		if(!$values) return $msg;

		$recipients = explode(",", $values);
		
		foreach($recipients as $recipient)
		{
			if(!preg_match('/\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\.[A-Z]{2,4}\b/i', $recipient))
			{
				if($msg) $msg .= "<br>";
				$msg .= "<warning>$recipient</warning> is an invalid email address.<br>";
			}
		}
	}
	
	/**
	 * Either get the sent as $_GET param or
	 * called from action handler
	 * 
	 * @param string $sent
	 * @return string|Ambigous <string>
	 */
	function getInfoMsg($sent = null, $dashboard = false)
	{
		if(!$sent)
		{
			$sent = checkNumeric($_GET["sent"]);
			if(!$sent) return "";
		}
		
		$messages = $this->getSendResultMessages($dashboard);
		
		return $messages[$sent];
	}
	
	function getSendResultMessages($dashboard = false)
	{
		$class = strtolower($this->getClassName());
		
		if(!$dashboard)
		{
			return array(
				questionnaire_sent				=>	"Your $class requests have been sent.",
				questionnaire_send_failed		=>	"Your $class requests could not be sent.",
				questionnaire_opened 			=>	"Your $class has been opened to respondents.",
				questionnaire_no_additional		=>	"There were no additional recipients who did not already receive the $class request.",
				questionnaire_reminder_sent		=>	"Reminder emails have been sent.",
				questionnaire_reminder_failed	=>	"Reminder emails could not be sent.",
				questionnaire_test_sent			=>	"A test email has been sent.",
				questionnaire_test_failed		=>	"A test email coult not be sent."
				);
		}
		else
		{
			return array(
					questionnaire_sent				=>	"Requests sent",
					questionnaire_send_failed		=>	"Requests failed",
					questionnaire_opened 			=>	"$class opened",
					questionnaire_no_additional		=>	"No new additional",
					questionnaire_reminder_sent		=>	"Reminders sent",
					questionnaire_reminder_failed	=>	"Reminders failed",
					questionnaire_test_sent			=>	"Test email sent",
					questionnaire_test_failed		=>	"Test email failed"
			);
		}
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
		$item_id = ($item->$itemPk) ? $item->$itemPk : 0;
		$component_name = $this->getComponentName();
		$item_label = $item->prettifyClassName();
		$send_test_email_dialog = $this->getSendTestEmailDialog();
		$send_test_email_handler = $this->getSendTestEmailHandler();
		$send_additional_handler = $this->getSendAdditionalHandler();
		
		$open_handler = $this->getOpenHandler();
		$reminder_dialog = $this->getReminderDialog();
		$close_handler = $this->getCloseHandler();
		$send_page_identifier = $this->getSendPageIdentifier();
		$recipients_dialog = $this->getRecipientsDialog();
		$message_select_dialog = $this->getMessageSelectDialog();
		$info_msg = $this->getInfoMsg();

		ob_start();
			?>
<script type="text/javascript" src="/components/questionnaire/js/questionnaire_send.js"></script>
<script type="text/javascript">
var questionnaireSendMgr;

window.addEvent('domready', function()
{
	questionnaireSendMgr = new QuestionnaireSendManager(
		'<?php echo $itemPk ?>', 
		<?php echo $item_id ?>,
		'<?php echo $component_name ?>', 
		'<?php echo $item_label ?>',
		'<?php echo $send_test_email_dialog ?>',
		'<?php echo $send_test_email_handler ?>',
		'<?php echo $send_additional_handler ?>',
		'<?php echo $open_handler ?>',
		'<?php echo $close_handler ?>',
		'<?php echo $reminder_dialog ?>',
		'<?php echo $send_page_identifier ?>',
		'<?php echo $recipients_dialog ?>',
		'<?php echo $message_select_dialog ?>',
		'<?php echo $info_msg ?>'
			  );
	
});
</script>
<?
		$script .= ob_get_contents();
		ob_end_clean();
		
		return $script;
	}	


} // AbstractQuestionnaireSendManager

?>