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


Fakoli::using("survey");
Fakoli::usingFeature("grouped_data_view");

$survey_id = checkNumeric($_GET["survey_id"]);

if(!$survey_id)
redirect("/survey_dashboard");

$redirect = "/survey_questions?survey_id=$survey_id";
$survey = new Survey($survey_id);
$survey->filter = new InclusionFilter("survey_id");
$form = new AutoForm($survey);
$form->button("Cancel", $redirect);

$templateQuestionSets = SurveyManager::getTemplateQuestionSets();
	
$surveyTemplateTable = buildTable($templateQuestionSets, "survey_template_question_sets", "survey_template_id");

$questionSets = SurveyManager::getQuestionSets($survey_id);
$surveyTable = buildTable($questionSets, "survey_question_sets", "survey_id");

$questionSelect = new DataListFieldRenderer($form, $surveyTemplateTable, "survey_template_question_select", "Select Survey Template Questions");

$templateQuestionSelect = new DataListFieldRenderer($form, $surveyTable, "survey_question_select", "Select Survey Questions");

$form->submitLabel = "Select";
$questionSelect->onPostProcess = selectQuestions;

$tabs = SurveyManager::surveyTabs($survey_id);
$tabs->page = "survey_questions";
$tabs->active = "Questions";

if ($method == "POST")
{
	if ($form->save())
	{
		redirect($redirect);
	}
}

$script .= $form->writeScript();
$tabs->writeHTML();
?>
<div id="tab_border">
<h3>Select Survey Questions</h3>
<?
$form->drawForm();

function buildTable($sets, $id, $pk)
{
	$table = new GroupedDataListView($sets, $id);

	$table->selector();
	$table->column("Question", "{Question.question}")
	->column("Options", array(CMSSurveyTableManager, formatOptions))
	->column("Required", array(CMSSurveyTableManager, formatRequired))
	;
	
	$table->emptyMessage = "There are no survey questions to select.";
	$table->mode = "fixed";
	$table->cssStyle = "width: 100%";

	foreach(array_keys($sets) as $value)
	{
		$class = ($pk == "survey_template_id") ? "SurveyTemplate" : "Survey";
		$survey = new $class($value);
		$table->group($survey->title, $value);
	}

	return $table;
}

function selectQuestions($questionSelect, $field)
{
	return SurveyManager::saveSelectedQuestions($questionSelect, $field);
}
?></div>
