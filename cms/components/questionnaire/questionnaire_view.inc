<?php
/**\addtogroup questionnaire
 * @{
 */



/**
 * Draw a view of the questionnaire from action handler from the
 * dashboard.
 * 
 * @author janice
 *
 */
class QuestionnaireView
{
	var $mgr; 				///< obj of class that extends AbstractQuestionnaireCreateManager
	var $form;				///< obj of class AutoForm		
	var $questionnaireView;
	
	function __construct($mgr)
	{
		$this->mgr = $mgr;

		$this->form = new AutoForm($mgr->item);
		$this->form->readOnlyForm = true;
		
		$this->questionnaireView = new QuestionnairePreview($mgr);
	}
	
	function writeScript()
	{
		$script .= $this->form->writeScript();
		$script .= $this->questionnaireView->writeScript();

		return $script;
	}
	
	function drawView()
	{
		$this->form->drawForm();
		echo "<p><label>Survey Questions</label></p>\n";
		$this->questionnaireView->drawView();
	}
}


class QuestionnaireSendView
{
	var $forms;
	var $sendMgr;
	var $response;
	
	/**
	 * Create a view of the questionnaire/survey in groups:
	 * Intro
	 * Email Recipients
	 * Sample Email
	 * Questionnaire
	 * Confirmation
	 * 
	 * @param instance of child class of AbstractQuestionnaireSendManager $sendMgr
	 */
	function __construct($sendMgr)
	{
		$this->sendMgr = $sendMgr;
			
		$this->response = $this->sendMgr->getSampleResponse();

		$this->forms["Introduction"] = $this->buildIntroForm();

		$this->forms["Email"] = $this->buildRecipientForm();

		$this->forms["Sample Email"] = $this->buildSampleEmailForm();

		$confirmationForm = $this->buildConfirmationForm();
		if($confirmationForm)
		{
			$this->forms["Confirmation"] = $confirmationForm;
		}
		
		$this->questionnaireView = new QuestionnairePreview($this->sendMgr->getCreateManager());
	}

	function buildIntroForm()
	{
		$item = clone $this->sendMgr->item;
		$pk = $item->getPrimaryKey();
		$id = $item->$pk;
		$item->filter = new InclusionFilter("title", "introduction", "start_date", "end_date");

		if(!$item->start_date)
		{
			$item->fields["start_date"] = String;
			$item->start_date = "N/A";
		}
		if(!$item->end_date)
		{
			$item->fields["end_date"] = String;
			$item->end_date = "Not specified";
		}

		$form = new AutoForm($item);
			
		$form->readOnlyForm = true;
		$identifier = $this->sendMgr->getQuestionnaireFormIdentifier();
		$form->data->set("edit_link", "<a href='$identifier?$pk={$id}'>(edit)</a>");

		return $form;
	}

	function buildRecipientForm()
	{
		$item = clone $this->sendMgr->item;
		$pk = $item->getPrimaryKey();
		$id = $item->$pk;
		$item->filter = new InclusionFilter("recipients", "additional_recipients", "cc_recipients", "sender_email");

		$item->sender_email = $this->sendMgr->getSenderEmail();

		if(!$item->recipients)
		{
			$item->recipients = "No recipients have been specified.";
		}
		else
		{
			$count = count(explode(",", $item->recipients));
			$item->fields["recipients"] = HTML;
			$label = ($count > 1) ? "recipients have" : "recipient has";
			$item->recipients = "$count survey $label been specified. <a class='button' href='#' onclick=\"questionnaireSendMgr.showRecipientsDialog(); return false\">View</a>\n";
		}

		if($item->additional_recipients)
		{
			$count = count(explode(",", $item->additional_recipients));
			$item->fields["additional_recipients"] = HTML;
			$label = ($count > 1) ? "recipients have" : "recipient has";
			$item->additional_recipients = "$count additional survey $label been specified. <a class='button' href='#' onclick=\"questionnaireSendMgr.showAdditionalRecipientsDialog(); return false\">View</a>\n";
		}

		if($item->cc_recipients)
		{
			$item->cc_recipients = implode(", ", explode(",", $item->cc_recipients));
		}
		else
		{
			$item->cc_recipients = "None";
		}

		if(!$item->subject)
		{
			$item->subject = " ";
		}

		if(!$item->message)
		{
			$item->message = " ";
		}
		$form = new AutoForm($item);
		$form->readOnlyForm = true;

		$form->getRenderer("recipients")->colspan = 1;
		$form->getRenderer("cc_recipients")->colspan = 1;
		$form->getRenderer("additional_recipients")->colspan = 1;

		$identifier = $this->sendMgr->getEmailForm();
		$form->data->set("edit_link", "<a href='{$identifier}?$pk={$id}'>(edit)</a>");

		return $form;
	}

	function buildSampleEmailForm()
	{
		$item = clone $this->sendMgr->item;
		$pk = $item->getPrimaryKey();
		$id = $item->$pk;
		$item->fields["message"] = HTML;

		$item->message = $this->sendMgr->getMessage();

		$response_class =  $this->sendMgr->getResponseClass();
		trace("**********item message {$item->message}", 3);
		$mergeMgr = new MergeCodeManager($this->response, $item->message, $response_class);
		$item->message = $mergeMgr->searchAndReplace($item->message);

		$item->sender_email = $this->sendMgr->getSenderEmail();

		if(!$item->subject) $item->subject = " ";
		$item->recipients = $this->response->email;
		$item->filter = new InclusionFilter("sender_email", "recipients", "subject", "message");
		$form = new AutoForm($item);
		$form->alias("recipients", "To:", "sender_email", "From:", "subject", "Subject:", "message", "Body:");
		$form->readOnlyForm = true;

		$form->getRenderer("recipients")->colspan = 1;
		$form->getRenderer("message")->colspan = 1;

		$form->labelCSS = "label_email";
		$form->formCSS = "email_preview";
		$form->valueCSS = "value_email";

		return $form;
	}

	function buildConfirmationForm()
	{
		$item = clone $this->sendMgr->item;
		
		if(!$item->hasField("confirmation_message")) return null;
		$pk = $item->getPrimaryKey();
		$id = $item->$pk;
		$item->filter = new InclusionFilter("confirmation_message");

		if($item->confirmation_message)
		{
			$response_class =  $this->sendMgr->getResponseClass();
			$mergeMgr = new MergeCodeManager($this->response, $item->confirmation_message, $response_class);
			$item->confirmation_message = $mergeMgr->searchAndReplace($item->confirmation_message);
		}

		$form = new AutoForm($item);

		if(!$form->data->get("confirmation_message"))
		{
			$form->data->set("confirmation_message", $this->sendMgr->formatDefaultConfirmationMessage());
			$form->alias("confirmation_message", "Confirmation Message (default)");
		}

		$form->readOnlyForm = true;
		$form->getRenderer("confirmation_message")->hideLabel = true;
		$identifier = $this->sendMgr->getConfirmationPage();
		$form->data->set("edit_link", "<a href='$identifier?{$pk}={$id}'>(edit)</a>");

		return $form;
	}

	function writeScript()
	{
		$script = $this->questionnaireView->writeScript();

		foreach($this->forms as $title => $form)
		{
			$script .= $form->writeScript();
		}

		return $script;
	}

	function drawView()
	{
		echo "<div class='questionnaire_preview'>\n";
		$form = $this->forms["Introduction"];
		echo "<fieldset>\n";
		echo "<legend>Introduction&nbsp;{$form->data->edit_link}</legend>\n";
		$form->drawForm();
		echo "</fieldset>\n";
			
		$form = $this->forms["Email"];
		echo "<fieldset>\n";
		echo "<legend>Email&nbsp;{$form->data->edit_link}</legend>\n";
		$this->sendMgr->writeSendValidationMsg();
		$form->drawForm();
		echo "</fieldset>\n";
			
		$form = $this->forms["Sample Email"];
		echo "<fieldset>\n";
		echo "<legend>Sample Email</legend>\n";
		$form->drawForm();
		echo "</fieldset>\n";

		$this->drawQuestionnaire();

		$form = $this->forms["Confirmation"];
		echo "<fieldset>\n";
		echo "<legend>Confirmation Message for Respondents&nbsp;{$form->data->edit_link}</legend>\n";
		$form->drawForm();
		echo "</fieldset>\n";
		echo "</div>\n";
	}


	function drawQuestionnaire()
	{
		$item = $this->sendMgr->item;
		$pk = $item->getPrimaryKey();
		$id = $item->$pk;
		$identifier = $this->sendMgr->getQuestionListIdentifier();
		
		echo "<fieldset>\n";
		echo "<legend>Survey Questions&nbsp;<a href='{$identifier}?{$pk}={$id}'>(edit)</a></legend>\n";
		$this->sendMgr->writeQuestionnaireValidationMsg();
		$this->questionnaireView->drawView();
		echo "</fieldset>\n";
	}
} // end QuestionnaireSendView


/**@}*/?>
