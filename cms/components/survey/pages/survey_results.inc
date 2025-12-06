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

/* survey_results.inc
 *
 * Description: Displays a table data list view of survey responses
 * with aggregate data results.
 *
 * author: Janice Gallant for Sonjara, Inc.
 *
 * date: 5/13/10
 */


Fakoli::using("questionnaire", "survey");
Fakoli::usingFeature("data_view");

$survey_id = checkNumeric($_GET["survey_id"]);

if(!$survey_id)
	redirect("index");

$survey = new Survey($survey_id);
$page->page_title = "Survey Responses for {$survey->title}";

$responses = $survey->Responses("ORDER BY last_modified DESC");

$mgr = new SurveyResultsManager($survey);
$sendMgr = new SurveySendManager($survey);

$table = new DataListView($responses, "surveyResponses");
$table->column("Token", array($mgr, 'formatResponseLink'), true)
	->column("Status", "{status:prettify}", true)
	->column("Date Submitted", "{last_modified|N/A}", true)
	->column("Include In Results", array($mgr, 'formatIncludeInResultsLink'), true, "text-align: center; width: 20px");
;

$table->emptyMessage = "There are no responses to this survey.";
$table->sortable = true;
$table->pageSize = 20;
$table->filter = false;
$table->cssStyle = "width: 100%";
$table->excelFile = "surveyResults.xls";
$table->rowId = true;

$tabs = SurveyManager::surveyTabs($survey_id);

$script .= $table->writeScript();
$script .= $mgr->writeScript();
$script .= $sendMgr->writeScript();

$tabs->writeHTML();
?>
<div id="survey_tab_border">
<h3>Survey Responses</h3>
<?
echo "<h3>".$survey->getTitle() . "</h3>\n";
// Use to show result of reminders & sent param from preview/manage page.
echo "<p id='action_result' class='questionnaire_action_result' style='display: none'></p>\n";

$table->drawView();

// anonymous response and send reminders, if applicable
echo $sendMgr->getResultsPageButtons();
?>
</div>
