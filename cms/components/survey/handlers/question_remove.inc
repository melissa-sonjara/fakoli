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
 * Description: Nondisplay script to unlink a question from a
 * survey question set by deleting the survey_question_xref
 * record. Can be called either from survey_template_questions
 * or survey_questions.
 *
 * author: Janice Gallant for Sonjara, Inc.
 *
 * date: 11/12/10
 *
 */

Fakoli::using("survey", "questionnaire");

$survey_question_xref_id = checkNumeric($_GET["survey_question_xref_id"]);
$survey_id = checkNumeric($_GET["survey_id"]);
$survey_question_id = checkNumeric($_GET["survey_question_id"]);

if(!$survey_id)
{
	Fakoli::end("Question Remove Failed - Unknown Survey");	
}
	
$survey = new Survey($survey_id);
$mgr = new SurveyCreateManager($survey);

// from question list page
if($survey_question_xref_id)
{
	$mgr->removeQuestionXref($survey_question_xref_id);
}
// from question form
else if($survey_question_id)
{
	$mgr->removeQuestion($survey_question_id);
}
else
{
	Fakoli::end("Question Remove - Unknown survey question");	
}

Fakoli::end("OK");
?>