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
 * Description: Form to add/update/delete a survey question.
 * Can be used for either defining standard questions for
 * question sets or to create a custom question within a survey.
 *
 * author: Janice Gallant for Sonjara, Inc.
 *
 * date: 10/17/2010
 *
 */

Fakoli::using("survey", "questionnaire");
Fakoli::usingFeature("auto_form");

$survey_question_id = checkNumeric($_GET["survey_question_id"]);
$survey_id = checkNumeric($_GET["survey_id"]);
$survey_template_id = checkNumeric($_GET["survey_template_id"]);

$question = new SurveyQuestion();

if(!$survey_question_id AND !$survey_id)
	redirect("/index");

if($survey_question_id)
{
	$question->load($survey_question_id);
}
else
{
	$question->num_rows = 3; // default
	$question->required = 0; // default
	global $xref;
	$xref = new SurveyQuestionXref();
	$xref->survey_id = $survey_id;
	$xref->sort_order = SurveyQuestionXref::getDefaultSortOrder("survey_id", $survey_id);
}

// check if question has been answered
$editable = $question->isEditable();
$isTemplateQuestion = $question->isTemplateQuestion();

$redirect = "/survey_questions?survey_id=$survey_id";

$form = SurveyQuestionForm::settings($question, $editable);

$typeSelect = new SelectFieldRenderer($form, "question_type_id", "Question Type", QuestionType::getQuestionTypeList());

if($editable AND !$isTemplateQuestion)
	$typeSelect->onChange = "new Survey().onChangeQuestionType";

if($isTemplateQuestion)
	$form->readOnlyForm = true;
elseif(!$editable)
	$form->readOnly("question_type_id", "options");

if($survey_question_id)
	$form->button("Remove", "/action/survey/question_remove?survey_id={$survey_id}&survey_question_id={$survey_question_id}");
$form->button("Cancel", $redirect);
$form->hide("question_name");

if(!$editable)
	$infoText = $question->getInfoText(survey_question_answered);
elseif($isTemplateQuestion)
	$infoText = $question->getInfoText(survey_question_template);

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

$script .= <<<ENDSCRIPT
	<script type="text/javascript">
	window.addEvent('domready', function()
	{
		 new Survey().questionFormSetup($question->question_type_id);
	});	
	</script>
ENDSCRIPT;

if($survey_id)
{
	$tabs->writeHTML();

	?>
<div id="tab_border"><?
}

echo "<b>{$infoText}</b>";

$form->drawForm();

function saveSurveyQuestionXref($obj)
{
	$question = $obj->data;
	global $xref;
	$xref->survey_question_id = $question->survey_question_id;
	$xref->save();
}
?></div>
