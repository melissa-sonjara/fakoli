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
 * Description: Data list view of questions in a questionnaire.
 * User can select a question to edit and change the question numbering.
 * 
 * author: Janice Gallant for Sonjara, Inc.
 * 
 * date: 11/9/09
 */

Fakoli::using("questionnaire");
Fakoli::usingFeature("auto_form");

$questionnaire_id = checkNumeric($_GET["questionnaire_id"]);

if (!$questionnaire_id) 
	redirect("questionnaire_dashboard");

$questionnaire = new Questionnaire($questionnaire_id);

$mgr = new QuestionnaireCreateManager($questionnaire);

$questions = $questionnaire->Questions("ORDER BY question_number");

$table = new DataListView($questions, "QuestionnaireQuestions");
$table->column("Question", array($mgr, formatQuestionLink))
	->column("Question Type", "{QuestionType.name}", false, "width: 15%")
	->column("Options", array(QuestionTableHelper, formatOptions))
	->column("Required", array(QuestionTableHelper, formatRequired))
	// For implementations with question xref, use "formatRemove"
	->column("Delete", array(QuestionTableHelper, formatDelete), false, "text-align: center")
;

$table->emptyMessage = "There are no questions defined.";
$mgr->enableDragReorder($table);
$table->sortable = false;
$table->cssStyle = "width: 100%";
$table->rowId = true;

$tabs = QuestionnaireCreateManager::getTabs($questionnaire_id);

$script .= $table->writeScript();
$script .= $mgr->writeScript();

$tabs->writeHTML();
?>

<div style="clear:left;border:solid 1px #000; padding: 4px;">
<?
$table->drawView();

echo "<p><a class='button' href='questionnaire_question_form?questionnaire_id={$questionnaire_id}'>Create a New Question</a>\n";
echo "&nbsp;&nbsp;<a class='button' href='questionnaire_preview?questionnaire_id={$questionnaire_id}'>Continue to Questionnaire Preview &raquo;&raquo;</a></p>\n";

?>
</div>