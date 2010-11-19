<?php
/*
 * survey_email.inc
 * 
 * Description: Link an email template to a specific questionnaire/survey.
 *  
 * author: Janice Gallant for Sonjara, Inc.
 * 
 * date: 6/23/10
 */


Fakoli::using("questionnaire", "survey");
Fakoli::usingFeature("auto_form");

$survey_id = checkNumeric($_GET["survey_id"]);
$error = checkNumeric($_GET["error"]);

if(!$survey_id)
	redirect("/survey_list");

$survey = new Survey($survey_id);
	
$survey->filter = new InclusionFilter("sender_email", "recipients", "message");
if(!$survey->sender_email)
{
	$surveySettings = SurveySetting::loadSettings();
	$survey->sender_email = $surveySettings->sender_email;
} 
$form = new AutoForm($survey);

$form->annotate("recipients", "(separate email addresses with a comma ',')");
$form->required("sender_email", "recipients", "message");
$form->getRenderer("message")->rows = 10;
$form->submitLabel = "Save";
$form->regexp("sender_email", "\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\.[A-Z]{2,4}\b", "Please supply a valid email address containing a domain name.");
$form->annotate("message", "Select from a set of boilerplate messages which you can then customize for this survey. 
<button onclick=\"surveyMessagePopup({$page->component_page_id}, this); return false\">Select a Message</button>&nbsp&nbsp;
<button onclick=\"javascript:modalPopup('Advanced Message Features', '/advanced_message_features_popup?class_name=SurveyResponse', '400px', 'auto'); return false\">Advanced Features</button>
");

if($error)
	$form->msg = $survey->getRequiredText($error, true);

$tabs = surveyTabs($survey_id);

$script .= $form->writeScript();
$script .= SurveyMergeCodeDialog::writeScript();
$script .= writeScript();

if ($method == "POST")
{
	if ($form->save())
	{
		$tabs->next();
	}
}


$styles .= "<link href=\"/templates/css/tabs.css\" rel=\"stylesheet\"/>";

$tabs->writeHTML();
?>
<div id="tab_border">
<button id="next_page_button" class="button" onclick="go('<?echo $tabs->getNextPage() ?>')">Next Page &raquo;</button>
<h3>Survey Email</h3>
<div id="inner_border">
<?
if($survey->isEditable())
	$form->drawForm();
else
	$form->drawReadOnly();
?>
</div></div><?


function writeScript()
{
	$surveyTemplates = query(SurveyTemplate);

	ob_start();
?>
<div class="dialog" id="surveyMessagePopup" style="width: 500px">
 <div class="dialog_header" id="surveyMessagePopupHeader">
  <div style="padding: 4px;">
   <div style="float: right">&nbsp;<a id='closeSurveyMessagePopup'">Close &times;</a></div>
   <span style="font-weight: bold">Survey Messages</span>
  </div>	
 </div>
 <div class="dialog_body">
<?
$table = SurveyMessageView::getView($surveyTemplates);
?>
  </div>
</div>

<script type='text/javascript'>

var surveyMessagePopupDialog;

function surveyMessagePopup(id, source)
{
	var popup = $('surveyMessagePopup');
	
	if (!surveyMessagePopupDialog)
	{
		surveyMessagePopupDialog = new FloatingDialog('surveyMessagePopup', {'closeLink': $('closeSurveyMessagePopup'), 'position': 'absolute'});
	}

	surveyMessagePopupDialog.targetID = id;
	surveyMessagePopupDialog.top = source.getCoordinates().top - 200;
	surveyMessagePopupDialog.left = source.getCoordinates().left;
	surveyMessagePopupDialog.show();
	
}


function updateSurveyMessage()
{
	var id = surveyMessagePopupDialog.targetID;
	var popup = $('surveyMessagePopup');

	var messageText;
	popup.getElements("input[type='radio']").each(function(e) 
			{ if (e.checked) 
				messageText = e.value;
			}
		);

	var message = $('Survey_form').message;
	message.innerHTML = messageText;
	surveyMessagePopupDialog.message = Class.Empty;
	surveyMessagePopupDialog.targetID = 0;
	surveyMessagePopupDialog.hide();
}

</script>
<?
		$script = ob_get_contents();
		ob_end_clean();
		$script .= $table->writeScript();
		return $script;
}

?>