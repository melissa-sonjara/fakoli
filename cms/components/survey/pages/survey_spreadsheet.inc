<?php
/*
 * Show the results for one survey in the format:
 *
 * Company Name    Question 1   Question 2...
 * Acme             Answer 1    Answer 2
 *
 */


Fakoli::using("questionnaire", "survey");
Fakoli::usingFeature("data_view");

$survey_id = checkNumeric($_GET["survey_id"]);

if(!$survey_id)
	redirect("index");

$survey = new Survey($survey_id);
$page->page_title = "Spreadsheet View of {$survey->title}";

$mgr = new SurveyResultsManager($survey);

$view = new QuestionnaireSpreadsheetView($mgr);

$tabs = SurveyManager::surveyTabs($survey_id);

$script .= $view->writeScript();
$script .= $mgr->writeScript();

$tabs->writeHTML();
?>
<div id="survey_tab_border">
<h3>Aggregated Survey Results for <?echo $survey->title?></h3>
<?php 
$mgr->drawEditQuestionNamesButton();

$view->drawView();
?>
</div>
