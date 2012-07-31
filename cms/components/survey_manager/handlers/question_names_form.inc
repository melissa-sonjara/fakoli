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
 * Show the survey questions and allow user to edit
 * the names that appear in the spreadsheet view.
 *
 */

Fakoli::using("survey", "survey_manager");
Fakoli::usingFeature("data_view");

$survey_id = checkNumeric($_GET["survey_id"]);

if(!$survey_id)
	ajaxReturn("unknown survey");

$survey = new Survey($survey_id);

$survey->filter = new InclusionFilter("survey_id");
$form = new AutoForm($survey, "POST", "/action/survey/question_names_form?survey_id=$survey_id", "QuestionNames_form");
$form->ajaxSubmit("function(result) {new SurveyResults().questionNamesFormResult(result);}", "function() {\$('{$form->id}_error').set('text','Failed to communicate with server'); }");

$xrefs = $survey->SurveyQuestionXrefs();
if(count($xrefs) > 0)
{
	$list = formatItems($xrefs, "{survey_question_id}", ",");
	$questions = Query::create(SurveyQuestion, "WHERE survey_question_id IN ($list)")->execute();
}
else
	$questions = array();

$table = new DataListView($questions, "SurveyQuestions");
$table->column("Question", "{question}")
	->column("Question Name", array(SurveyTableHelper, formatQuestionNameInputField), false)
	;

$table->emptyMessage = "There are no questions defined.";
$table->sortable = false;
$table->cssStyle = "width: 100%";

$questionSelect = new DataListFieldRenderer($form, $table, "survey_questions", "Survey Questions");
$form->submitLabel = "Save Question Names";
$questionSelect->onPostProcess = saveQuestionNames;

if(count($xrefs) == 0)
$form->readOnlyForm = true;

if ($method == "POST")
{
	if($form->save())
		ajaxReturn("OK");
	else
		ajaxReturn($form->msg);
}

echo $form->writeScript();

$form->drawForm();

function saveQuestionNames()
{
	global $_POST;

	foreach($_POST as $name => $value)
	{
		if (!strncmp($name, "question_", 9))
		{
			$id = substr($name, 9);
			$question = new SurveyQuestion();
			$question->survey_question_id = $id;
			$question->question_name = $value;
			$question->filter = new InclusionFilter("question_name");
			$question->save();
		}
	}

	return true;
}