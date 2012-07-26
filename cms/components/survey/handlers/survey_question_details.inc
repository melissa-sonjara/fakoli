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
 * Shows details of one survey question.
 */

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

if(!$survey_question_id AND !$survey_id)
	ajaxReturn("unknown question or survey");

$question = new SurveyQuestion($survey_question_id);

if($question->question_type_id != question_type_checklist)
	$question->fields["required"] = Boolean;

$question->filter = new ExclusionFilter("locked");

$form = new AutoForm($question, "POST", "/action/survey/survey_question_detail?survey_id=$survey_id&survey_question_id=$survey_question_id");

$form->getRenderer("num_rows")->size = 5;
$form->getRenderer("char_limit")->size = 5;

if($question->question_type_id == question_type_multi)
{
	$form->hide("char_limit", "num_rows");
}
elseif($question->question_type_id == question_type_rating)
{
	$form->hide("char_limit", "num_rows");
	$form->add(new StringFieldRenderer($form), "label_for_lowest");
	$form->add(new StringFieldRenderer($form), "label_for_highest");
	$form->add(new NumberFieldRenderer($form), "number_of_steps");
	$form->hide("options");
	$form = SurveyQuestionForm::setOptionsToRatings($form);
}
elseif($question->question_type_id == question_type_short)
{
	$form->hide("num_rows", "options");
}
elseif($question->question_type_id == question_type_free)
{
	$form->hide("options");
}
elseif($question->question_type_id == question_type_checklist)
{
	$form->hide("char_limit", "num_rows");
}
elseif($question->question_type_id == question_type_dropdown)
{
	$form->hide("char_limit", "num_rows");
}


$form->readOnlyForm = true;

if(!$question->isTemplateQuestion())
	$infoText = $question->getInfoText(survey_question_answered);
else
	$infoText = $question->getInfoText(survey_question_template);

echo $form->writeScript();

echo "<b>{$infoText}</b>";

$form->drawForm();
