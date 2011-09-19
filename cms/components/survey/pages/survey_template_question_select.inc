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

$survey_template_id = checkNumeric($_GET["survey_template_id"]);

if(!$survey_template_id)
	redirect("/survey_templates");

$surveyTemplate = new SurveyTemplate($survey_template_id);
$surveyTemplate->filter = new InclusionFilter("survey_template_id");
$form = new AutoForm($surveyTemplate);

$redirect = "/survey_template_questions?survey_template_id=$survey_template_id";

$questionSets = SurveyManager::getQuestionSets("survey_template_id", $survey_template_id);

$table = new GroupedDataListView($questionSets, "templates");

$table->selector();
$table->column("Question", "{Question.question}")
	->column("Options", array(CMSSurveyTableManager, formatOptions))
	->column("Required", array(CMSSurveyTableManager, formatRequired))
	;
	
$table->emptyMessage = "There are no survey questions to select.";
$table->mode = "fixed";
$table->cssStyle = "width: 100%";

foreach(array_keys($questionSets) as $survey_template_id)
{
	$surveyTemplate = new SurveyTemplate($survey_template_id);
	$table->group($surveyTemplate->title, $survey_template_id);
}

$questionSelect = new DataListFieldRenderer($form, $table, "survey_template_question_select", "Select Survey Questions");
$form->submitLabel = "Select";
$questionSelect->onPostProcess = selectQuestions;

$form->button("Cancel", $redirect);

$tabs = SurveyTemplateManager::surveyTemplateTabs($survey_template_id);
$tabs->page = "survey_template_questions";
$tabs->active = "Survey Template Questions";

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
<h3>Select Survey Template Questions</h3>
<?
$form->drawForm();

function selectQuestions($questionSelect, $field)
{
	return SurveyManager::saveSelectedQuestions($questionSelect, $field);
}

?></div>
