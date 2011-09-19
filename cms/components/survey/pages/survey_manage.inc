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
 *
 * Title: survey_manage.inc
 *
 * Description: Preview of a survey response request. Provide
 * applicable button options such as public view, close survey, and
 * reopen survey.
 *
 * author: Janice Gallant for Sonjara, Inc.
 *
 * date: 6/23/10
 *
 */


Fakoli::using("questionnaire", "survey", "email");
Fakoli::usingFeature("data_view");

$survey_id = checkNumeric($_GET["survey_id"]);

if (!$survey_id)
redirect("/survey_list");

$survey = new Survey($survey_id);

if($survey->status == "not sent")
$validate = new ValidateSurvey($survey);
$view = new SurveyManageView($survey, ($validate) ? true : false);

$tabs = SurveyManager::surveyTabs($survey_id, false);

$page->page_title = "Manage Survey";

$tabs->writeHTML();
?>
<div id="tab_border">
<button id="next_page_button" class="button"
	onclick="go('<?echo $tabs->getNextPage() ?>')">Next Page &raquo;</button>
<h3>Manage Survey</h3>
<div id="inner_border">
<h2><? echo $survey->title ?></h2>
<p><? echo $survey->getStatusInfoText(); ?></p>
<? if ($survey->status > "not sent")
{
	?>
<div id='prog_bar'>
<h4>Percentage of Surveys Received</h4>
	<? echo SurveyManager::getSurveyProgressBar($survey); ?></div>
	<?
}
$view->drawView();
?></div>
</div>
