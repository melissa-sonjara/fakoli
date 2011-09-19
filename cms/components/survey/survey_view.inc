<?php

/*
 * Title: survey.inc
 *
 * Description: Classes and functions for email questionnaire
 * surveys.
 *
 * author: Janice Gallant for Sonjara, Inc.
 *
 * date: 5/13/2010
 *
 * Updated labels for Survey Template (formerly called Questionnaire) (S. Hubbard, 7 June 2010)
 *
 */


Fakoli::using("email");
Fakoli::usingFeature("tab_bar", "data_view", "status_tree", "auto_form");



class SurveyTemplateView
{
	var $mgr;
	var $questionnaireView;
	var $surveyForm;

	function SurveyTemplateView($surveyTemplate)
	{
		$this->mgr = new SurveyTemplateManager($surveyTemplate);
		$this->questionnaireView = new QuestionnairePreview($this->mgr);
		$this->surveyForm = new AutoForm($surveyTemplate);
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
		$this->mgr = new SurveyManager($survey);
		$this->questionnaireView = new QuestionnairePreview($this->mgr);

		$surveyResponse = new SurveyResponse();
		$surveyResponse->survey_id = $this->survey->survey_id;
		if($this->emailTemplate)
		$this->emailMerge = new EmailManager($surveyResponse, $this->emailTemplate->template_id, $this->survey->sender_email, $this->survey->message);

		$c_survey = clone($survey);
		$mergeMgr = new MergeCodeManager($surveyResponse, $survey->message, "SurveyResponse");
		$c_survey->message = $mergeMgr->searchAndReplace($survey->message);
		$c_survey->filter = new InclusionFilter("title", "introduction", "message", "sender_email", "recipients", "start_date", "end_date");
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
		$this->surveyForm = new AutoForm($c_survey);
		$this->surveyForm->readOnlyForm = true;

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

class SurveyManageView extends SurveyPreviewSendView
{
	var $emailMerge;
	var $deleteUrl = "";
	var $editUrl;
	var $reviewUrl;
	var $publicUrl;
	var $statusUrl;
	var $iconView = "/fakoli/images/icon_view.gif";
	var $iconPublic = "/fakoli/images/icon_view_public.gif";
	var $iconEdit = "/fakoli/images/icon_edit.gif";
	var $iconOpen = "/fakoli/images/icon_reopen.gif";
	var $iconClose = "/fakoli/images/icon_close.gif";
	var $iconDelete = "/fakoli/images/icon_delete.gif";
	var $iconResults = "/fakoli/images/icon_survey_results.gif";
	var $iconReminder = "/fakoli/images/email.gif";


	function SurveyManageView($survey)
	{
		parent::SurveyPreviewSendView($survey, true);
	}

	function drawView()
	{
		$this->drawManageLinks();
		parent::drawView();
	}


	/*
	 * Draw a:link button options on Manage Survey tab
	 * view
	 * public view
	 * edit
	 * close
	 * delete
	 */
	function drawManageLinks()
	{
		$pk = $this->survey->getPrimaryKey();
		$value = $this->survey->$pk;
		$this->editUrl = "survey_form?survey_id={$value}";
		$this->publicUrl = "survey_response_intro?survey_id={$value}";
		$this->reviewUrl = "survey_data?survey_id={$value}";

		$responseCount = $this->mgr->getResponseCount();
		$status = $this->survey->status;

		echo "<div id='manage_survey'>\n";

		if($status != "not sent")
		{
			if(responseCount > 0)
			{
				echo $this->getOneManageLink($this->reviewUrl,
				$this->iconView, "View Results");
			}
				
			echo $this->getOneManageLink($this->publicUrl,
			$this->iconPublic, "Public View");
		}

		if($status == "not sent")
		{
			echo $this->getOneManageLink($this->editUrl,
			$this->iconEdit, "Edit");
		}
		elseif($status == "open")
		{
			echo $this->getOneManageLink("", $this->iconClose,
				"Close", "new Survey().closeSurvey($value)");
				
			if(count($this->survey->getNonResponders()) > 0)
			{
				echo $this->getOneManageLink("", $this->iconReminder,
					"Send Reminders", "new Survey().showSurveyReminderDialog($value)");
			}
		}
		elseif($status == "closed")
		{
			echo $this->getOneManageLink("#", $this->iconOpen,
				"Reopen", "new Survey().reopenSurvey($value)");
		}

		if($status != "open")
		{
			echo $this->getOneManageLink("#", $this->iconDelete,
				"Delete", "new Survey().deleteSurvey($value, $responseCount)");
		}
			
		echo "</div>\n";
	}

	function getOneManageLink($url, $icon, $label, $onclick = "")
	{
		if(!$onclick)
			$html = "<a href='{$url}'>";
		else
			$html .= "<a href='#' onclick=\"$onclick; return false;\">";
		$html .= "<img src=\"{$icon}\" style='border: none;'/>&nbsp;$label</a>\n";

		return $html;
	}
}


class SurveyMessageView
{
	function SurveyMessageView()
	{

	}

	static function getView($surveyTemplates)
	{

		$table = new DataListView($surveyTemplates, "surveyTemplates");
		$table->column("Title", "<input style='border: none' type='radio' name='message' value='{message}'>{title}</input>", false, "width: 25%")
		->column("Message", "{message}", false, "width: 50%");

		$table->emptyMessage = "There are no survey messages defined.";
		$table->sortable = false;
		$table->pageSize = 0;
		$table->paginate = false;
		$table->filter = false;
		$table->cssStyle = "width: 90%";

		$table->drawView();
		?>
<button class='button'
	style='float: left; margin-bottom: 5px; margin-top: 5px'
	onclick='updateSurveyMessage()'>Select Message</button>
		<?php
		return $table;
	}
}



class SurveyIntroductionView
{
	function SurveyIntroductionView()
	{

	}

	static function getView($surveyTemplates)
	{

		$table = new DataListView($surveyTemplates, "surveyTemplates");
		$table->column("Title", "<input style='border: none' type='radio' name='message' value='{message}'>{title}</input>", false, "width: 25%")
		->column("Introduction", "{introduction}", false, "width: 50%");

		$table->emptyMessage = "There are no survey introductions defined.";
		$table->sortable = false;
		$table->pageSize = 0;
		$table->paginate = false;
		$table->filter = false;
		$table->cssStyle = "width: 90%";

		$table->drawView();
		?>
<button class='button'
	style='float: left; margin-bottom: 5px; margin-top: 5px'
	onclick='updateSurveyIntroduction()'>Select Introduction</button>
		<?php
		return $table;
	}
}

class SurveyQuestionForm
{
	function SurveyQuestionForm()
	{

	}

	static function settings($question)
	{
		$form = new AutoForm($question);
		$form->required("question");
		$form->annotate("options", "<br><span class='annotate'>List each option on a separate line.</span>");
		$form->annotate("question_name", "<span class='annotate'>For spreadsheet report column headings</span>");
		$form->hide("locked");
		if(!$question->survey_question_id)
			$form->onSaveComplete = saveSurveyQuestionXref;

		$form->allowDelete = false;

		$form->alias("char_limit", "Character Limit for Answer",
			"num_rows", "Number of Rows for Answer",
			"options", "Options for Answer",
			"required", "Answer Required"
			);

			$form->getRenderer("question")->height = "100px";

			$form->add(new StringFieldRenderer($form), "label_for_lowest");
			$form->add(new StringFieldRenderer($form), "label_for_highest");
			$form->add(new NumberFieldRenderer($form), "number_of_steps");

			$form->getRenderer("num_rows")->size = 5;
			$form->getRenderer("char_limit")->size = 5;

			if($question->survey_question_id && question_type_rating)
				$form = SurveyQuestionForm::setOptionsToRatings($form);
			else
				$form = SurveyQuestionForm::setDefaultRatings($form);

			$form->labelCSS = "survey_question_form_label";
				
			return $form;
	}

	static function setRatingsToOptions()
	{
		global $_POST;

		$options = $_POST["label_for_lowest"] . "\r\n";
		$options .= $_POST["label_for_highest"] . "\r\n";
		$options .= $_POST["number_of_steps"] . "\r\n";

		$_POST["options"] = $options;
	}

	static function setOptionsToRatings($form)
	{
		list($from, $to, $steps) = explode("\n", $form->data->get("options"));

		$form->data->set("label_for_lowest", $from);
		$form->data->set("label_for_highest", $to);
		$form->data->set("number_of_steps", $steps);

		return $form;
	}

	static function setDefaultRatings($form)
	{
		$form->data->set("label_for_lowest", "Lowest");
		$form->data->set("label_for_highest", "Highest");
		$form->data->set("number_of_steps", 5);

		return $form;
	}
}
?>