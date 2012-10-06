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

$question_id = checkNumeric($_GET["question_id"]);
$questionnaire_id = checkNumeric($_GET["questionnaire_id"]);

$question = new Question();
	
if($question_id)
{
	$question->load($question_id);
	$questionnaire = $question->Questionnaire();
	$questionnaire_id = $questionnaire->questionnaire_id; // needed for tabs
	// set pseudo field
	$question->editable = $question->isEditable();
}
elseif($questionnaire_id)
{
	$questionnaire = new Questionnaire($questionnaire_id);
	$question->questionnaire_id = $questionnaire_id;
	$question->setDefaultValues();
	$mgr = new QuestionnaireCreateManager($questionnaire);
	$question->question_number = $mgr->getNextSortOrder();
}
else
{
	redirect("questionnaire_dashboard");
}

$redirect = "questionnaire_questions?questionnaire_id=$questionnaire_id";
$form = new AutoForm($question);
$form = QuestionFormHelper::questionTypeSetup($form, $question->editable);

$form->hide("question_id", "questionnaire_id", "question_name", "question_number");
$form->required("question");
$form->getRenderer("question")->height = "100px";
$form->annotate("options", "<br><span class='annotate'>List each option on a separate line.</span>");
$form->allowDelete = $question->allowDelete();
$form->button("Cancel", $redirect);

$tabs = QuestionnaireCreateManager::getTabs($questionnaire_id);
$tabs->page = "questionnaire_questions";

if ($method == "POST")
{
	if ($form->save())
	{
		redirect($redirect);
	}
}

$script .= $form->writeScript();
$script .= QuestionFormHelper::writeScript($form);

$tabs->writeHTML();

?>
<div style="clear:left;border:solid 1px #000; padding: 4px;">
<button style="float: right" class="button" 
onclick="go('<?echo $tabs->getNextPage() ?>')">  Next Page &raquo;  </button>
<?
echo "<h3>{$questionnaire->title}</h3>";

if(!$question->editable)
{
	echo $question->formatEditableMessage();
}
$form->drawForm();
?>
</div>