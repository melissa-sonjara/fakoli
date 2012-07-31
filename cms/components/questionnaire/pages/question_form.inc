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

Fakoli::using("questionnaire");
Fakoli::usingFeature("auto_form");

require_once realpath(dirname(__FILE__)."/../questionnaire_tabs.inc");
require_once realpath(dirname(__FILE__)."/../question_set.inc");

$question_id = checkNumeric($_GET["question_id"]);
$questionnaire_id = checkNumeric($_GET["questionnaire_id"]);

$question = new Question();

if(!$question_id AND !$questionnaire_id)
	redirect("/questionnaire_list");
	
if($question_id)
{
	$question->load($question_id);
	$questionnaire_id = $question->questionnaire_id; // needed for tabs
}
else
{
	$question->questionnaire_id = $questionnaire_id;
	$question->question_number = queryValue(Question, "MAX(question_number)", "WHERE questionnaire_id=$questionnaire_id") + 1;
	$question->question_type_id = 4; // free text
	$question->num_rows = 3; // default
	$question->required = 0; // default
}


$questionnaire = new Questionnaire($questionnaire_id);
// check if question has been answered
$editable = $question->isEditable();
if(!$editable)
	$question->filter = new ExclusionFilter("question_type_id", "options");		

$redirect = "question_list?questionnaire_id=$questionnaire_id";
$form = new AutoForm($question);
$form->hide("question_id", "questionnaire_id", "question_name");
$form->required("question_number", "question");
$form->annotate("options", "<br><span class='annotate'>List each option on a separate line.</span>");
$form->button("Cancel", $redirect);

$form->getRenderer("question")->height = "100px";

if($editable)
{
	$form->allowDelete = true;
	$form->add(new  RatingQuestionFieldRenderer($form), "label_for_lowest");
	$form->add(new StringFieldRenderer($form), "label_for_highest");
	$form->add(new NumberFieldRenderer($form), "number_of_steps");	
	$typeSelect = new SelectFieldRenderer($form, "question_type_id", "Question Type", QuestionType::getQuestionTypeList());
	$typeSelect->onChange = "new Question().onChangeQuestionType";
}

$tabs = questionnaireTabs($questionnaire_id);
$tabs->page = "question_list";
$tabs->active = "Question List";

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
		 new Question().setup($question->question_type_id, '{$form->id}');
	});	
	</script>
ENDSCRIPT;

$tabs->writeHTML();

?>
<div style="clear:left;border:solid 1px #000; padding: 4px;">
<button style="float: right" class="button" 
onclick="go('<?echo $tabs->getNextPage() ?>')">  Next Page &raquo;  </button>
<?
echo "<h3>{$questionnaire->title}</h3>";
if(!$editable)
{
	echo "<h5>{$question->getNotEditableHTML()}</h5>";
}
$form->drawForm();
?>
</div>