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
 * 
 * Title: question_form.php
 * 
 * Description: Form to add/update/delete a question in a 
 * questionnaire.
 * 
 * author: Janice Gallant for Sonjara, Inc.
 * 
 * date: 6/09
 * 
 */

require_once realpath(dirname(__FILE__)."/../../include/config.inc");
require_once realpath(dirname(__FILE__)."/../../cms/datamodel/questionnaire.inc");
require_once realpath(dirname(__FILE__)."/../../framework/auto_form.inc");
require_once realpath(dirname(__FILE__)."/../../components/questionnaire/questionnaire_tabs.inc");
require_once realpath(dirname(__FILE__)."/../../components/questionnaire/questionnaire_form.inc");
require_once realpath(dirname(__FILE__)."/../../components/questionnaire/question_set.inc");

$question_key = checkNumeric($_GET["question_key"]);
$questionnaire_key = checkNumeric($_GET["questionnaire_key"]);

$question = new Question();

if(!$question_key AND !$questionnaire_key)
	redirect("questionnaire_list.php");
	
if($question_key)
{
	$question->load($question_key);
	$subtitle = "Edit a Question";
	$questionnaire_key = $question->questionnaire_key; // needed for tabs
}
else
{
	$question->questionnaire_key = $questionnaire_key;
	$question->question_number = queryValue(Question, "MAX(question_number)", "WHERE questionnaire_key=$questionnaire_key") + 1;
	$question->num_rows = 3; // default
	$question->required = 0; // default
	$subtitle = "Add a Question";
}


$questionnaire = new Questionnaire($questionnaire_key);
// check if question has been answered
$editable = $question->isEditable();
if(!$editable)
	$question->filter = new ExclusionFilter("question_type", "options");		

$redirect = "question_list.php?questionnaire_key=$questionnaire_key";
$form = new AutoForm($question);
$form->hide("question_key", "questionnaire_key");
$form->required("question_number", "question");
$form->annotate("options", "<span style='font-weight: normal;font-size: 8pt'><br/>For Multiple-Choice or CheckList questions, list each option on a separate line.<br/>For Rating questions, list the text for lowest rating, text for highest rating and number of steps on separate lines</span>");
$form->annotate("required", "<span style='font-weight: normal;font-size: 8pt'>enter 0 if not required, 1 if required, and for checklist, enter the count of required checkboxes</span>");
$form->annotate("char_limit", "<span style='font-weight: normal;font-size: 8pt'>For freetext questions only</span>");
$form->annotate("num_rows", "<span style='font-weight: normal;font-size: 8pt'>For freetext questions only</span>");
$form->button("Cancel", $redirect);

if($editable)
	$form->allowDelete = true;
	
$form->alias("char_limit", "Character Limit for Answer Field", 
			"num_rows", "Number of Rows to Display for Answer");

if($editable)
	$typeSelect = new SelectFieldRenderer($form, "question_type", "Question Type", $questionTypes);

$tabs = questionnaireTabs($questionnaire_key);
$tabs->page = "question_list.php";
$tabs->active = "Question List";

if ($method == "POST")
{
	if ($form->save())
	{
		redirect($redirect);
	}
}

$script .= $form->writeScript();

require_once realpath(dirname(__FILE__)."/../../templates/begin_page.inc");
$tabs->writeHTML();

?>
<div style="clear:left;border:solid 1px #000; padding: 4px;">
<button style="float: right" class="button" 
onclick="go('<?echo $tabs->getNextPage() ?>')">  Next Page &raquo;  </button>
<?
echo "<h3>{$questionnaire->title}: $subtitle</h3>";
if(!$editable)
{
	echo "<h5>{$question->getNotEditableHTML()}</h5>";
}
$form->drawForm();
 ?>
</div>
<?
require_once realpath(dirname(__FILE__)."/../../templates/end_page.inc");
?>