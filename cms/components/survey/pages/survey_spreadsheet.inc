<?php
/*
 * Show the results for one survey in the format:
 *
 * Company Name    Question 1   Question 2...
 * Acme             Answer 1    Answer 2
 *
 */


Fakoli::using("questionnaire", "survey", "survey_manager");
Fakoli::usingFeature("data_view");

$survey_id = checkNumeric($_GET["survey_id"]);

if(!$survey_id)
	redirect("index");

$survey = new Survey($survey_id);
$page->page_title = "Spreadsheet View of {$survey->title}";

$mgr = new SurveyResultsManager($survey);

$view = new SurveySpreadsheetView($mgr);

$tabs = SurveyManager::surveyTabs($survey_id);

$script .= $view->writeScript();

$tabs->writeHTML();
?>
<div id="tab_border">
<h3>Aggregated Survey Results for <?echo $survey->title?></h3>
<p><a class="button" href="#"
	onclick="new SurveyResults().showQuestionNamesDialog(<?php echo $survey_id ?>); return false;">Edit
Column Headings</a></p>
<div id="survey_spreadsheet" class="scroll"
	style="padding: 4px; padding-top: 10px; width: 100%; height: 500px; overflow: auto">
<?
$view->drawView();
?></div>
</div>
