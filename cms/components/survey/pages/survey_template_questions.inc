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

if(!$survey_template_id)
	redirect("/survey_templates");

$surveyTemplate = new SurveyTemplate($survey_template_id);

$mgr = new SurveyTemplateManager($surveyTemplate);
$listView = new QuestionSet($mgr);
$listView->button("Select from Existing Questions", "/survey_template_question_select?survey_template_id=$survey_template_id");

$tabs = surveyTemplateTabs($survey_template_id, false);
$styles .= "<link href=\"/templates/css/tabs.css\" rel=\"stylesheet\"/>";

if ($method == "POST")
{
	if ($_POST["question_order_form"])
	{
		$mgr->reorderQuestions();
		// refresh
		redirect("/survey_template_questions?survey_template_id=$survey_template_id");
	}
}

$script .= $listView->writeScript();

$tabs->writeHTML();

?>
<div id="tab_border">
<p>Select the questions for this template. You can select from a list of existing questions or create new ones. Deleting a question from this template does not delete the question from the database.</p>
<?php
$listView->drawList();
?>
</div>
<br/>
<a href="/survey_templates">Return to Survey Template List</a>
