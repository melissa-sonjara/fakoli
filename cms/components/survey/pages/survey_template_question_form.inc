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
 * Description: Form to add/update/delete a question within
 * a question set.
 *
 * author: Janice Gallant for Sonjara, Inc.
 *
 * date: 10/17/2010
 *
 */

Fakoli::using("survey", "questionnaire");
Fakoli::usingFeature("auto_form");

global $survey_template_id;
global $xref;

$survey_question_id = checkNumeric($_GET["survey_question_id"]);
$survey_template_id = checkNumeric($_GET["survey_template_id"]);

$question = new SurveyQuestion();

if(!$survey_question_id AND !$survey_template_id)
	redirect("/index");

if($survey_question_id)
{
	$question->load($survey_question_id);
}
else
{
	$question->locked = true;
	$question->num_rows = 3; // default
	$question->required = 0; // default
	$xref = new SurveyQuestionXref();
	$xref->survey_template_id = $survey_template_id;
	$xref->sort_order = SurveyQuestionXref::getDefaultSortOrder("survey_template_id", $survey_template_id);
}

// check if question has been answered
$editable = $question->isEditable();
if(!$editable)
	$question->filter = new ExclusionFilter("question_type_id", "options");

$redirect = "/survey_template_questions?survey_template_id=$survey_template_id";

$form = SurveyQuestionForm::settings($question);

if($question->isEditable())
{
	$typeSelect = new SelectFieldRenderer($form, "question_type_id", "Question Type", QuestionType::getQuestionTypeList());
	$typeSelect->onChange = "new Survey().onChangeQuestionType";
}

if($survey_question_id)
$form->button("Remove", "/action/survey/question_remove?survey_template_id={$survey_template_id}&survey_question_id={$survey_question_id}");

$form->button("Cancel", $redirect);

if(!$editable)
$infoText = $question->getInfoText(survey_question_answered);

$tabs = SurveyTemplateManager::surveyTemplateTabs($survey_template_id);
$tabs->page = "survey_template_questions";
$tabs->active = "Survey Template Questions";

if ($method == "POST")
{
	$question_type_id = $_POST["question_type_id"];
	if($question_type_id == question_type_rating)
	SurveyQuestionForm::setRatingsToOptions();

	if ($form->save())
	{
		redirect($redirect);
	}
}

$script .= $form->writeScript();

$script .= <<<ENDSCRIPT
	<script type="text/javascript">
	window.addEvent('domready', function()
	{
		 new Survey().setQuestionTypeOptions();
	});	
	</script>
ENDSCRIPT;
$tabs->writeHTML();
?>
<div style="clear: left; border: solid 1px #000; padding: 4px;">
<button style="float: right" class="button"
	onclick="go('<?echo $tabs->getNextPage() ?>')">Next Page &raquo;</button>
<?


echo "<b>{$infoText}</b>";

$form->drawForm();
?></div>
<?php

function saveSurveyQuestionXref($form)
{
	global $survey_template_id;

	$question = $form->data;
	global $xref;
	$xref->survey_question_id = $question->survey_question_id;
	$xref->save();
}
?>