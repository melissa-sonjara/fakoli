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

 /* survey_data.inc
 * 
 * Description: Displays aggregate results of a survey.
 *  
 * author: Janice Gallant for Sonjara, Inc.
 * 
 * date: 5/13/10
 */


Fakoli::using("questionnaire", "survey");
Fakoli::usingFeature("data_view");

require_once realpath(dirname(__FILE__)."/../survey_view.inc");
require_once realpath(dirname(__FILE__)."/../results_view.inc");
require_once realpath(dirname(__FILE__)."/../results_report.inc");

$survey_id = checkNumeric($_GET["survey_id"]);

if(!$survey_id)
	redirect("index");
	
$survey = new Survey($survey_id);
$view = new SurveyReport($survey);

$tabs = surveyTabs($survey_id);
$menuView = new SurveyMenuView('survey_form');
$survey_top_nav = $menuView->getView();
$dataTabs = surveyDataTabs($survey_id);

//$script .= $view->writeScript();
$styles .= "<link href='templates/css/tabs.css' rel='stylesheet'/>\n";
$styles .= "<link href='templates/css/survey.css' rel='stylesheet'/>\n";

$tabs->writeHTML();
?>
<div id="tab_border">
<h3>Aggregated Survey Results for <?echo $survey->title?></h3>
<div id="inner_border" style="padding: 0;">
<?
$dataTabs->writeHTML();
?>
<div style="padding: 4px;">
<?
$view->drawView();
?>
</div></div></div><?
?>