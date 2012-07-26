<?php
/*
 * Lets admin users select from standard questions to create a
 * set of questions.
 *
 * @author Janice Gallant for Sonjara, Inc.
 *
 * 10/17/2010
 */

Fakoli::using("survey");
Fakoli::usingFeature("data_view");

$survey_template_id = checkNumeric($_GET["survey_template_id"]);

if(!$survey_template_id)
redirect("survey_templates");

$surveyTemplate = new SurveyTemplate($survey_template_id);
$surveyTemplate->filter = new InclusionFilter("survey_template_id");
$form = new AutoForm($surveyTemplate);

$xrefs = $surveyTemplate->SurveyQuestionXrefs();

$table = new DataListView($xrefs, "SurveyQuestions");
$table->column("Question", "<a href=\"survey_template_question_form?survey_template_id={survey_template_id}&survey_question_id={survey_question_id}\">{Question.question}</a>")
	->column("Sort Order", "<input type='text' name=\"xref_{survey_question_xref_id}\"
		value=\"{sort_order}\" size=\"4\"/>", false, "width: 10%")
	->column("Options", array(CMSSurveyTableManager, formatOptions))
	->column("Required", array(CMSSurveyTableManager, formatRequired))
	->column("Remove", "<a href=\"#\" onclick=\"new Survey().removeSurveyQuestion({survey_question_xref_id}); return false;\">
			<img class=\"icon\" alt=\"delete\" src=\"/fakoli/images/delete.gif\"></a>")
	;

$table->emptyMessage = "There are no questions defined.";
$table->sortable = false;

$questionSelect = new DataListFieldRenderer($form, $table, "survey_template_questions", "Survey Template Questions");
$form->submitLabel = "Save Order";
$questionSelect->onPostProcess = reOrderQuestions;

if(count($xrefs) == 0)
	$form->readOnlyForm = true;

$form->button("Create a New Question", "survey_template_question_form?survey_template_id=$survey_template_id");
$form->button("Select from Existing Questions", "survey_template_question_select?survey_template_id=$survey_template_id");

$tabs = SurveyTemplateManager::surveyTemplateTabs($survey_template_id, false);

if ($method == "POST")
{
	if($form->save())
	{
		redirect("survey_template_questions?survey_template_id=$survey_template_id");
	}
}

$script .= $form->writeScript();

$tabs->writeHTML();

?>
<div id="tab_border">
<h3><?php echo $surveyTemplate->title ?></h3>
<p>Select the questions for this template. You can select from a list of
existing questions or create new ones. Removing a question from this
template does not delete the question from the database unless the
question is not linked to any other template or survey.</p>
<?php
$form->drawForm();

if(count($xrefs) == 0)
{
	?>
<p><a class='button'
	href='survey_template_question_form?survey_template_id=<?php echo $survey_template_id ?>'>Create
a New Question</a>&nbsp;&nbsp; <a class='button'
	href="survey_template_question_select?survey_template_id=<?php echo $survey_template_id ?>">Select
from Existing Questions</a></p>
	<?php
}
?></div>
<?php


function reOrderQuestions($tableSelect, $field)
{
	global $_POST;
	// reorder
	foreach($_POST as $name => $value)
	{
		if (!strncmp($name, "xref_", 5))
		{
			$id = substr($name, 5);
			$xref = new SurveyQuestionXref($id);
			$xref->saveSortOrder($value);
		}
	}
	// renumber
	$value = 1;
	$xrefs = Query::create(SurveyQuestionXref, "WHERE survey_template_id=:survey_template_id ORDER BY sort_order")
	->bind(":survey_template_id", $tableSelect->parent->data->survey_template_id)
	->execute();

	foreach($xrefs as $xref)
	{
		$xref->saveSortOrder($value);
		$value++;
	}
}


?>