<?php
/*
 * Lets admin users select from standard questions to create a 
 * set of questions.
 * 
 * @author Janice Gallant for Sonjara, Inc.
 * 
 * 10/17/2010
 */

Fakoli::using("survey", "questionnaire");
Fakoli::usingFeature("auto_form");

require_once realpath(dirname(__FILE__)."/../../questionnaire/question_set.inc");
require_once realpath(dirname(__FILE__)."/../survey_template_view.inc");

$survey_template_id = checkNumeric($_GET["survey_template_id"]);

$surveyTemplate = new SurveyTemplate();

if($survey_template_id)
	$surveyTemplate->load($survey_template_id);
	
$surveyTemplate->filter = new ExclusionFilter("status");
$form = new AutoForm($surveyTemplate);
$form->required("name", "message");
$form->annotate("message", "Create a boilerplate message for the email survey requests.
<button onclick=\"mergeCodePopup({$page->component_page_id}, this); return false\">Advanced Features</button>");

$tabs = surveyTemplateTabs($survey_template_id, false);
$styles .= "<link href=\"/templates/css/tabs.css\" rel=\"stylesheet\"/>";

if ($method == "POST")
{
	if($form->save())
	{
		$tabs->queryString = "survey_template_id={$form->data->survey_template_id}";
		$tabs->next();
	}
}

$script .= $form->writeScript();
$script .= SurveyMergeCodeDialog::writeScript();

$tabs->writeHTML();
?>
<div id="tab_border">
<?php
$form->drawForm();
?></div>