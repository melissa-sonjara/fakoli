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

require_once realpath(dirname(__FILE__)."/../survey_view.inc");

$survey_id = checkNumeric($_GET["survey_id"]);

if(!$survey_id)
	redirect("index");
	
$survey = new Survey($survey_id);

$responses = $survey->Responses("ORDER BY last_modified DESC");

function getStatus($response)
{
	$statusTitle = $response->statusTypes[$response->status];
	if ($response->isSubmitted())
		return "<a href=\"survey_response_view?response_id={$response->response_id}\">$statusTitle</a>\n";
	else
		return $statusTitle;
}

function getSubmitDate($response)
{
	return ($response->last_modified > 0) ? formatDate($response->last_modified) : "";
}


$table = new DataListView($responses, "surveyResponses");
$table->column("Email", "{email}", true);
$table->column("Status", getStatus, true);
$table->column("Date Submitted", getSubmitDate, true);

$table->emptyMessage = "There are no responses to this survey.";
$table->sortable = true;
$table->pageSize = 20;
$table->filter = false;
$table->cssStyle = "width: 100%";
$table->excelFile = "surveyResults.xls";

$tabs = surveyTabs($survey_id);
$menuView = new SurveyMenuView('survey_form');
$survey_top_nav = $menuView->getView();

$styles .= "<link href='templates/css/tabs.css' rel='stylesheet'/>\n";

$script .= $table->writeScript();

$tabs->writeHTML();
?>
<div id="tab_border">
<h3>Survey Responses</h3>
<?
echo "<h3>".$survey->getTitle() . "</h3>\n";
if($survey->program_id)
	echo "<h4>".$survey->Program()->name . "</h4>\n";

$table->drawView();
?>
</div><?
?>
