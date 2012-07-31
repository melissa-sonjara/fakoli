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


Fakoli::using("questionnaire", "survey", "survey_manager");
Fakoli::usingFeature("data_view");

$survey_id = checkNumeric($_GET["survey_id"]);

if(!$survey_id)
	redirect("index");

$survey = new Survey($survey_id);
$page->page_title = "Survey Responses for {$survey->title}";

$responses = $survey->Responses("ORDER BY last_modified DESC");
$recipientCount = $survey->getRecipientCount();
$responseCount = $survey->getResponseCount();

$table = new DataListView($responses, "surveyResponses");
$table->column("Token", array(SurveyTableHelper, formatResponseLink), true)
	->column("Status", "{status:prettify}", true)
	->column("Date Submitted", array(SurveyTableHelper, formatResponseSubmitDate), true)
;

$table->emptyMessage = "There are no responses to this survey.";
$table->sortable = true;
$table->pageSize = 20;
$table->filter = false;
$table->cssStyle = "width: 100%";
$table->excelFile = "surveyResults.xls";

$tabs = SurveyManager::surveyTabs($survey_id);

$script .= $table->writeScript();

$tabs->writeHTML();
?>
<div id="tab_border">
<h3>Survey Responses</h3>
<?
echo "<h3>".$survey->getTitle() . "</h3>\n";
echo "<p id='survey_reminder_result' class='error' style='display: 'none'></p>\n";

$table->drawView();
if($survey->isOpen() AND $recipientCount > 0 AND $responseCount < $recipientCount)
echo "<p><a class=\"button\" href='#' onclick=\"new SurveyResponse().showSurveyReminderDialog($survey_id); return false;\">Send Survey Reminders</a></p>\n";
?></div>
