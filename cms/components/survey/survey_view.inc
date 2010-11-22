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

function surveyTabs($key = "")
{
	$tabs = getSurveyTabs($key);
	$qs = ($key) ? "survey_id=$key" : "";
	$tabBar = new TwoLevelTabBar($tabs, $qs);
		
	return $tabBar;
}

function getSurveyTabs($key)
{
	if(!$key)
		$formTitle = "Create a Survey";
	else
	{
		$survey = new Survey($key);
		$formTitle = ($survey->isEditable()) ? "Edit Survey" : "Survey Details";
	}

	$images[0] = "<img src='/fakoli/images/tab_create_survey.gif' style='display: inline-block;vertical-align: middle; border: none'/>&nbsp;";	
	$images[1] = "<img src='/fakoli/images/tab_manage_survey.gif' style='display: inline-block;vertical-align: middle; border: none'/>&nbsp;";
	$images[2] = "<img src='/fakoli/images/tab_view_results.gif' style='display: inline-block;vertical-align: middle; border: none'/>&nbsp;";
	
	
	$tabs = array(	$images[0] . $formTitle	=>
					array(	"Introduction"			=>	"survey_form",
					  		"Questions"				=>	"survey_questions",
						  	"Email"					=>	"survey_email",
							"Preview/Send"	=>	"survey_preview"),
					
					$images[1]. "Manage Survey"	=>
					array(	"My Survey"				=>	"survey_manage"),

					$images[1] . "View Results"	=>
					array(	"Survey Responses"		=>	"survey_results",
							"Survey Data"			=>	"survey_data"),
						);
						
					
	return $tabs;
}



function surveyDataTabs($key = "")
{
	$tabs = array(	"Graphical View"	=>	"survey_data",
					"Tabular View"		=>	"survey_data_table");
	
	$tabBar = new TabBar("datatabs", $tabs, "survey_id=$key");
	return $tabBar;
}

function responseTabs($key = "", $token = "")
{
	$tabs = getResponseTabs();
	$qs = ($key) ? "survey_id=$key&token=$token" : "";
	$tabBar = new TabBar("tabs", $tabs, $qs);

	return $tabBar;
}

function getResponseTabs()
{
		$tabs = array(	"Survey Form"		=>	"survey_response_form",
					 	"Preview &amp; Submit"	=>	"survey_response_preview"
					);
	return $tabs;
}


function getProgramList()
{
	global $user;
	
	if(checkRole('admin'))
		$programs = query(Program, "ORDER BY name");
	else	
		$programs = $user->Programs("ORDER BY name");
	
	$programList = array();
	if(count($programs) > 0)
	{
		foreach($programs as $program)
			$programList[$program->program_id] = $program->name;
	}

	// puts this at the end
	array_push($programList, "not applicable");

	return $programList;
}


class SurveyView
{
	var $survey;
	var $emailMerge;
	var $validate;
	
	function SurveyView($survey, $validationCheck = true)
	{
		$this->survey = $survey;
		$surveyResponse = new SurveyResponse();
		$surveyResponse->survey_id = $this->survey->survey_id;
		if($this->emailTemplate)
			$this->emailMerge = new EmailManager($surveyResponse, $this->emailTemplate->template_id, $this->survey->sender_email, $this->survey->message);
		if ($validationCheck)
			$this->validate = new ValidateSurvey($survey);	
	}
	
	function drawView()
	{
		$this->drawSurveyDetails();
		$this->drawQuestionnaire();
		$this->drawEmailTemplate();
	}
	
	function drawSurveyDetails()
	{
		echo $this->getSurveyDetails();
	}
	
	/* shared with status block */
	function getSurveyDetails()
	{	
		$startDate = ($this->survey->start_date > 0) ? formatDate($this->survey->start_date) : "not sent";
		$endDate = ($this->survey->end_date > 0) ? formatDate($this->survey->end_date) : "not specified";
		
		$html = "<p><label>Title </label>{$this->survey->title}</p>\n";	
		$html .= "<p><label>Introduction </label>". formatAsHTML($this->survey->introduction) ."</p>\n";
	 	$html .= "<p><label>Start Date </label>$startDate</p>\n";
		$html .= "<p><label>End Date </label>$endDate</p>\n";
		return $html;
	}
	
	function drawQuestionnaire()
	{
		if($this->validate)
		{
			$error = $this->validate->validateQuestionnaire();
			
			if($error)
				echo $this->survey->getRequiredText($error, true);
		}

		$mgr = new SurveyManager($this->survey);
		$questionnaireView = new QuestionnairePreview($mgr);
		echo "<p><label>Survey Questions</label></p>\n";
		$questionnaireView->drawView();
	}
	
	function drawEmailTemplate()
	{
		if($this->validate)
		{
			$error = $this->validate->validateEmail();
			if($validate->survey)
				$this->survey = $validate->survey; // identify incorrect email addresses

			if($error)
				echo $this->survey->getRequiredText($error, true);
		}

		$recipients = ($this->survey->recipients) ? $this->survey->recipients : "not specified";
		echo "<p><label>Recipient List </label>{$recipients}</p>\n";
		$emailFrom = ($this->survey->sender_email) ? $this->survey->sender_email : "not specified";
		echo "<p><label>Email From </label>". $emailFrom ."</p>\n";
		
		if($this->emailMerge)
		{
			echo "<p><label>Email Subject </label>". formatAsHTML($this->emailMerge->subject) ."</p>\n";
			echo "<p><label>Email Message</label></p>\n";
			echo "<p>". formatAsHTML($this->emailMerge->message) ."</p>\n";
		}	
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
		/* share paths and icons with status block */
		$block = new SurveyStatusBlockView($this->survey);	
		$status = $this->survey->status;
		$rtn = "&return=1";
		 		
		if($status > survey_not_sent)
		{
			$html .= $this->getOneManageLink($block->reviewUrl, 
				$block->iconView, "View");
			
			$html .= $this->getOneManageLink($block->publicUrl, 
				$block->iconPublic, "Public View");	
		}
		
		if($status == survey_not_sent)
		{
			$html .= $this->getOneManageLink($block->editUrl,
				$block->iconEdit, "Edit");
		}
		elseif($status == survey_open) 
		{
			$html .= $this->getOneManageLink($block->statusUrl . survey_closed . $rtn,
					$block->iconClose, "Close");
		}
		elseif($status == survey_closed)
		{
			$html .= $this->getOneManageLink($block->statusUrl . survey_open . $rtn,
					$block->iconOpen, "Reopen");
		}
		
		if($status != survey_open)
		{
			$html .= $this->getOneManageLink($block->deleteUrl . $rtn,
					$block->iconDelete, "Delete");
		}
		
		echo "<div id='manage_survey'\n";
		echo $html;
		echo "</div>\n";
	}
	
	function getOneManageLink($url, $icon, $label)
	{
		$html = "<a href=\"{$url}\"><img src=\"{$icon}\" style='border: none;'/>&nbsp;$label</a>\n";
		
		return $html;
	}
}






function sendSurveyEmails($survey)
{
	global $config;
	
	$recipients = explode(",", $survey->recipients);
	$surveySetting = SurveySetting::loadSettings ();
	
	if(count($recipients) > 0)
	{
		foreach($recipients as $recipient)
		{
			$response = query(SurveyResponse, "WHERE email='$recipient' AND survey_id={$survey->survey_id}");
			if(count($response) == 0)
			{
				$response = SurveyResponse::generateToken($survey->survey_id, $recipient);
				$response->save();
			}
			else
				$response = $response[0];
			// Store info in email template data structure but
			// don't save it.
			$emailTemplate = new EmailTemplate();
			$emailTemplate->sender_email = $survey->sender_email;
			$emailTemplate->recipients = $recipient;
			$emailTemplate->subject = $surveySetting->email_subject;
			$emailTemplate->message = $survey->message . $surveySetting->message_footer;
			$emailTemplate->class_name = "SurveyResponse";
			$emailHandler = new EmailManager($response, $emailTemplate);
			$sent = $emailHandler->sendEmail();	
		}
	}
	
	$survey->setStatus(survey_open); 
}

function onDeleteSurveyCascade($survey)
{
	$responses = $survey->Responses();
	if(count($responses) > 0)
	{
		foreach($responses as $response)
		{
			$answers = $response->Answers();
			if(count($answers) > 0)
			{
				foreach($answers as $answer)
					$answer->delete();
			}
			$response->delete();
		}
	}
	$survey->filter = new InclusionFilter("deleted");
	$survey->deleted = true;
	$survey->save();
	
}



function getSurveyProgressBar($survey)
{
	$percentage = 0;
	$total = queryValue(SurveyResponse, "COUNT(1)", "WHERE survey_id={$survey->survey_id}");
	if($total > 0)
	{
		$responseCount = $survey->getResponseCount();
		$percentage = round($responseCount / $total * 100, 0);
	}
	
	$html = "<div class='col2'>\n";
	$html .= $percentage . "%";
	if ($responseCount) $html .= " <span style='font-size: 8pt'>($responseCount&nbsp;of&nbsp;$total)</span>";
	$html .= "</div>\n";
	$html .= "<div class='progress_bar'>\n";
	$html .= "<div class='progress' style='width: $percentage%'>\n";
	$html .= "</div></div>\n";
	
	return $html;
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
  		<button class='button' style='float: left; margin-bottom: 5px; margin-top: 5px' onclick='updateSurveyMessage()'>Select Message</button>
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
  		<button class='button' style='float: left; margin-bottom: 5px; margin-top: 5px' onclick='updateSurveyIntroduction()'>Select Introduction</button>
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
		$form->annotate("options", "<br><span class='annotate'>For Multiple-Choice or CheckList questions, list each option on a separate line.<br/>For Rating questions, list the text for lowest rating, text for highest rating and number of steps on separate lines.</span>");
		$form->annotate("required", "<span class='annotate'>enter 0 if not required, 1 if required, and for checklist, enter the count of required checkboxes</span>");
		$form->annotate("char_limit", "<span class='annotate'>For freetext questions only</span>");
		$form->annotate("num_rows", "<span class='annotate'>For freetext questions only</span>");
		$form->hide("locked");
		$form->onSaveComplete = saveSurveyQuestionXref;

		$form->allowDelete = false;
	
		$form->alias("char_limit", "Character Limit for Answer Field", 
			"num_rows", "Number of Rows to Display for Answer");
	
		return $form;
	}
}
?>