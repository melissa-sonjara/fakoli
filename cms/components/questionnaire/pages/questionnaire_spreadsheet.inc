<?php
/*
 * Show the results for one survey in the format:
 *
 * Company Name    Question 1   Question 2...
 * Acme             Answer 1    Answer 2
 *
 */


Fakoli::using("questionnaire");
Fakoli::usingFeature("data_view");

$questionnaire_id = checkNumeric($_GET["questionnaire_id"]);

if(!$questionnaire_id)
	redirect("/index");

$questionnaire = new Questionnaire($questionnaire_id);
$page->page_title = "Spreadsheet View of {$questionnaire->title}";

$mgr = new QuestionnaireResultsManager($questionnaire);

$view = new QuestionnaireSpreadsheetView($mgr);

$tabs = QuestionnaireCreateManager::getTabs($questionnaire_id);

$script .= $view->writeScript();
$script .= $mgr->writeScript();

$tabs->writeHTML();
?>
<div id="questionnaire_tab_border">
<?php
$mgr->drawEditQuestionNamesButton();

$view->drawView();
?>
</div>
