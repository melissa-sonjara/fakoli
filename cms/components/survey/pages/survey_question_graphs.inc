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
 * Show graphical view of aggregated results for each question across all the
 * surveys the question was used in.
 */

Fakoli::using("questionnaire", "survey");

$questions = Query::create('SurveyQuestion', "WHERE question_type_id != 7 AND survey_question_id IN (SELECT survey_question_id FROM survey_question_xref WHERE survey_id IN (SELECT survey_id FROM survey where deleted=false))")->execute();

$table = new DataListView($xrefs, "SurveyQuestions");
$table->column("Question", "{Question.question}")
	->column("Question Type", "{QuestionType.name}", false, "width: 15%")
	->column("Options", array('QuestionTableHelper', 'formatOptions'))
	->column("Answers", "")
	->column("Graph", "")
	;

$table->emptyMessage = "There are no questions defined.";
$table->sortable = true;
$table->cssStyle = "width: 100%";

$script .= $table->writeScript();

$table->drawView();

?>
