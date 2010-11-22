<?php
/**************************************************************

 Copyright (c) 2007,2008 Sonjara, Inc

 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:

 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.

 Except as contained in this notice, the name(s) of the above 
 copyright holders shall not be used in advertising or otherwise 
 to promote the sale, use or other dealings in this Software 
 without prior written authorization.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.

*****************************************************************/

/*
 * survey_form.inc
 * 
 * Description: Link an email template to a specific questionnaire/survey
 * and set of recipients. Provide a button for user to send the emails.
 *  
 * author: Janice Gallant for Sonjara, Inc.
 * 
 * date: 5/13/10
 */


Fakoli::using("questionnaire", "survey");
Fakoli::usingFeature("auto_form");

$survey_id = checkNumeric($_GET["survey_id"]);
$error = checkNumeric($_GET["error"]);

$title = "Survey Request";

$survey = new Survey();

if($survey_id)
	$survey->load($survey_id);
else
{
	$survey->user_id = $user->user_id;
	$survey->sender_email = $config["email_from"];
}
	
$survey->filter = new InclusionFilter("title", "introduction", "end_date", "user_id");
$form = new AutoForm($survey);

if($error)
	$form->msg = $survey->getRequiredText($error, true);

$form->button("Cancel", "survey_list");
$form->required("title");
$form->hide("user_id");
$form->submitLabel = "Save";
$form->annotate("end_date", "date on which survey closes to respondents");
$form->annotate("introduction", "Instructions for respondents. You can write your own introductory message or choose from templated introductions.
<button onclick=\"surveyIntroductionPopup({$page->component_page_id}, this); return false\">Select an Introduction</button>&nbsp&nbsp;
");
$form->getRenderer("introduction")->rows = 8; 
$dateRenderer = $form->getRenderer("end_date");
$dateRenderer->required = false;

$tabs = surveyTabs($survey_id);

$styles .= "<link href=\"/templates/css/tabs.css\" rel=\"stylesheet\"/>";

$script .= $form->writeScript();
$script .= writeScript();

if ($method == "POST")
{
	if ($form->save())
	{
		$tabs->queryString = "survey_id={$form->data->survey_id}";
		$tabs->next();
	}
}

$tabs->writeHTML();
?>
<div id="tab_border">
<h3>Survey Introduction</h3>
<?
if($survey->isEditable())
	$form->drawForm();
else
	$form->drawReadOnly();
?>
</div><?


function writeScript()
{
	$surveyTemplates = query(SurveyTemplate, "WHERE introduction != ''");

	ob_start();
?>
<div class="dialog" id="surveyIntroductionPopup" style="width: 500px">
 <div class="dialog_header" id="surveyIntroductionPopupHeader">
  <div style="padding: 4px;">
   <div style="float: right">&nbsp;<a id='closeSurveyIntroductionPopup'">Close &times;</a></div>
   <span style="font-weight: bold">Survey Introductions</span>
  </div>	
 </div>
 <div class="dialog_body">
<?
$table = SurveyIntroductionView::getView($surveyTemplates);
?>
  </div>
</div>

<script type='text/javascript'>

var surveyIntroductionPopupDialog;

function surveyIntroductionPopup(id, source)
{
	var popup = $('surveyIntroductionPopup');
	
	if (!surveyIntroductionPopupDialog)
	{
		surveyIntroductionPopupDialog = new FloatingDialog('surveyIntroductionPopup', {'closeLink': $('closeSurveyIntroductionPopup'), 'position': 'absolute'});
	}

	surveyIntroductionPopupDialog.targetID = id;
	surveyIntroductionPopupDialog.top = source.getCoordinates().top;
	surveyIntroductionPopupDialog.left = source.getCoordinates().left;
	surveyIntroductionPopupDialog.show();
	
}


function updateSurveyIntroduction()
{
	var id = surveyIntroductionPopupDialog.targetID;
	var popup = $('surveyIntroductionPopup');

	var introText;
	popup.getElements("input[type='radio']").each(function(e) 
			{ if (e.checked) 
				introText = e.value;
			}
		);

	var introduction = $('Survey_form').introduction;
	introduction.innerHTML = introText;
	surveyIntroductionPopupDialog.introduction = Class.Empty;
	surveyIntroductionPopupDialog.targetID = 0;
	surveyIntroductionPopupDialog.hide();
}

</script>
<?
		$script = ob_get_contents();
		ob_end_clean();
		$script .= $table->writeScript();
		return $script;
}

?>