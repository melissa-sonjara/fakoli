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

/**
 * The questionnaire component does not use this handler because
 * questions are not linked through an xref. It is provided here
 * so that implementing classes and copy the format into 
 * their components;
 */

Fakoli::using("questionnaire");

$questionnaire_id = checkNumeric($_GET["questionnaire_id"]);
$question_id = checkNumeric($_GET["question_id"]);
$question_xref_id = checkNumeric($_GET["question_xref_id"]);

if(!$questionnaire_id)
{
	ajaxReturn("Question Remove Failed - Unknown Questionnaire");	
}
	
$questionnaire = new Questionnaire($questionnaire_id);
$mgr = new QuestionnaireCreateManager($questionnaire);

// from question list page
if($question_xref_id)
{
	$mgr->removeQuestionXref($question_xref_id);
}
// from question form
else if($question_id)
{
	$mgr->removeQuestion($question_id);
}
else
{
	ajaxReturn("Question Remove - Unknown questionnaire/question");	
}

ajaxReturn("OK");
?>