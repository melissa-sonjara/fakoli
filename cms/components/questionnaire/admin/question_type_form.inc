<?php
/*
 * Title: question_type_form.inc
 * 
 * Description: Form to add/update/delete a question type used
 * in questionnaires.
 * 
 * author: Janice Gallant for Sonjara, Inc.
 * 
 * date: 8/5/10
 * 
 */

Fakoli::using("questionnaire");
Fakoli::usingFeature("auto_form");

$question_type_id = checkNumeric($_GET["question_type_id"]);

$questionType = new QuestionType();
	
if($question_type_id)
{
	$questionType->load($question_type_id);
}
else
{
	$questionType->sort_order = queryValue(QuestionType, "MAX(sort_order)") + 1;
}

$redirect = "/admin/question_types";
$form = new AutoForm($questionType);
$form->required("sort_order", "name");
$form->button("Cancel", $redirect);

if(!$question_type_id)
	$form->annotate("class_name", "the name of the class that renders the question in the code files");
else
	$form->readOnly("class_name");
$form->annotate("options", "whether an options list is applicable to this question type");
$form->annotate("char_limit", "if applicable");
$form->annotate("num_rows", "if applicable");

$form->alias("name", "Question Type Name", 
			"char_limit", "Character Limit for Answer Field", 
			"num_rows", "Number of Rows to Display for Answer");

if ($method == "POST")
{
	if ($form->save())
	{
		redirect($redirect);
	}
}

$script .= $form->writeScript();
?>
<p>This form is intended for Systems Administrators. If a question type is added to this list, the class to to render the question type must be added to the questionnaire component's question_renderer.inc file.</p>
<?php
$form->drawForm();
