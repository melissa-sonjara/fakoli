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
	redirect("/survey_templates");

$surveyTemplate = new SurveyTemplate($survey_template_id);

$xrefs = $surveyTemplate->SurveyQuestionXrefs();

$table = new DataListView($xrefs, "SurveyQuestions");
$table->column("Question", "<a href=\"/survey_template_question_form?survey_template_id={survey_template_id}&survey_question_id={survey_question_id}\">{Question.question}</a>")
		->column("Sort Order", formatSortOrderInputField, false, "width: 10%")
		->column("Options", formatOptions)
		->column("Remove", formatRemoveButton)
		;
	  	
$table->emptyMessage = "There are no questions defined.";
$table->sortable = false;
$table->cssStyle = "width: 80%";

$tabs = surveyTemplateTabs($survey_template_id, false);
$styles .= "<link href=\"/templates/css/tabs.css\" rel=\"stylesheet\"/>";

if ($method == "POST")
{
	reOrderQuestions($xrefs, $survey_template_id);
	// refresh
	redirect("/survey_template_questions?survey_template_id=$survey_template_id");
}

$script .= $table->writeScript();

$tabs->writeHTML();


?>
<div id="tab_border">
<h3><?php echo $surveyTemplate->title ?></h3>
<p>Select the questions for this template. You can select from a list of existing questions or create new ones. Removing a question from this template does not delete the question from the database unless the question is not linked to any other template or survey.</p>
<form style='display: inline' name="question_order_form" method="POST" action="">
<?php
$table->drawView();
?>
<br><br><input class='button' type="submit" name="reorder" value="&nbsp;Save Order&nbsp;"/>&nbsp;&nbsp;
<a class='button' href='/survey_template_question_form?survey_template_id=<?php echo $survey_template_id ?>'>Create a New Question</a>&nbsp;&nbsp;
<a class='button' href="/survey_template_question_select?survey_template_id=<?php echo $survey_template_id ?>">Select from Existing Questions</a>
</form></div>
<?php 

function formatSortOrderInputField($xref)
{
	return "<input type='text' name='xref_{$xref->survey_question_xref_id}' 
		value='{$xref->sort_order}' size='4'/>"; 
}

function formatOptions($xref)
{
	return preg_replace("/\r\n/", "<br>", $xref->Question()->options);
	
}

function formatRemoveButton($xref)
{
	$confirm = "Are you sure you want to remove this question?";	
	$url = "/action/survey/question_remove?survey_template_id={$xref->survey_template_id}&survey_question_id={$xref->survey_question_id}";
	
	$link = "if (confirm('".jsSafe($confirm)."')) go('{$url}'); return false;";
	
	return "<input type='button' class='button' onclick=\"$link\" value=\"Remove\"/>";
}

function reOrderQuestions($xrefs, $survey_template_id)
{
	global $_POST;
	// reorder
	foreach($_POST as $name => $value)
	{
		if (!strncmp($name, "xref_", 5))
		{
			$id = substr($name, 5);
			$xref = new SurveyQuestionXref($id);
	 		$xref->filter = new InclusionFilter("sort_order");
			$xref->sort_order = $value;
			$xref->save();
		}
	}
	// renumber
	$value = 1;
	$xrefs = query(SurveyQuestionXref, "WHERE survey_template_id = {$survey_template_id} ORDER BY sort_order");
	foreach($xrefs as $xref)
	{
		$xref->filter = new InclusionFilter("sort_order");
		$xref->sort_order = $value;
		$xref->save();
		$value++;
	}
}


?>