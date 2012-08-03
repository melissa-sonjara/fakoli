<?php

class SurveyView
{
	var $mgr;
	var $questionnaireView;
	var $surveyForm;

	function SurveyView($survey)
	{
		$this->mgr = new SurveyCreateManager($survey);
		$this->questionnaireView = new QuestionnairePreview($this->mgr);
		$this->surveyForm = new AutoForm($survey);
		$this->surveyForm->hide("status");
		$this->surveyForm->readOnlyForm = true;
	}

	function writeScript()
	{
		$script = $this->questionnaireView->writeScript();
		$script .= $this->surveyForm->writeScript();

		return $script;
	}

	function drawView()
	{
		$this->surveyForm->drawForm();
		echo "<p><label>Survey Questions</label></p>\n";
		$this->questionnaireView->drawView();
	}

}


/*
 * Display survey on preview & send page.
 */
class SurveyPreviewSendView
{
	var $mgr; // class SurveyManager
	var $survey;
	var $questionnaireView;
	var $surveyForm;
	var $validate;

	function SurveyPreviewSendView($survey)
	{
		$this->survey = $survey;
		$this->mgr = new SurveyCreateManager($survey);
		$this->questionnaireView = new QuestionnairePreview($this->mgr);

		$surveyResponse = new SurveyResponse();
		$surveyResponse->survey_id = $this->survey->survey_id;
		if($this->emailTemplate)
		$this->emailMerge = new EmailManager($surveyResponse, $this->emailTemplate->template_id, $this->survey->sender_email, $this->survey->message);

		$c_survey = clone($survey);
		$mergeMgr = new MergeCodeManager($surveyResponse, $survey->message, "SurveyResponse");
		$c_survey->message = $mergeMgr->searchAndReplace($survey->message);
		$c_survey->filter = new InclusionFilter("title", "introduction", "confirmation_message", "message", "sender_email", "recipients", "start_date", "end_date");
		if(!$c_survey->recipients)
			$c_survey->recipients = "Not specified";
		if(!$c_survey->start_date)
		{
			$c_survey->fields["start_date"] = String;
			$c_survey->start_date = "N/A";
		}
		if(!$c_survey->end_date)
		{
			$c_survey->fields["end_date"] = String;
			$c_survey->end_date = "Not specified";
		}
		$form = new AutoForm($c_survey);
		$form->readOnlyForm = true;
		
		$this->surveyForm = $form;
		$this->validate = new ValidateSurvey($survey);
	}

	function writeScript()
	{
		$script = $this->questionnaireView->writeScript();
		$script .= $this->surveyForm->writeScript();

		return $script;
	}

	function drawView()
	{
		if(!$this->validate->validateEmail())
			$this->validate->warning();

		$this->surveyForm->drawForm();
		$this->drawQuestionnaire();
	}

	function drawQuestionnaire()
	{
		$this->validate->msg = "";

		if(!$this->validate->validateQuestionnaire())
		$this->validate->warning();

		echo "<p><label>Survey Questions</label></p>\n";
		$this->questionnaireView->drawView();
	}
}
