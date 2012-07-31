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
 * survey_questions.inc
 *
 * Description: Survey Question tab of survey data entry
 * interface. Lets user select an existing survey template
 * or write a custom one.
 *
 * author: Janice Gallant for Sonjara, Inc.
 *
 * date: 6/23/10
 */

Fakoli::using("survey", "survey_manager");
Fakoli::usingFeature("data_view");

$survey_id = checkNumeric($_GET["survey_id"]);

if(!$survey_id)
	redirect("survey_dashboard");

$survey = new Survey($survey_id);
$page->page_title .= " for {$survey->title}";

$survey->filter = new InclusionFilter("survey_id");
$form = new AutoForm($survey);

$xrefs = $survey->SurveyQuestionXrefs();

$table = new DataListView($xrefs, "SurveyQuestions");
$table->column("Question", "<a href='survey_question_form?survey_id={survey_id}&survey_question_id={survey_question_id}'>{Question.question}</a>")
	->column("Sort Order", "<input type='text' name=\"xref_{survey_question_xref_id}\"
		value=\"{sort_order}\" size=\"4\"/>", false, "width: 10%")
	->column("Options", array(SurveyQuestionTableHelper, formatOptions))
	->column("Required", array(SurveyQuestionTableHelper, formatRequired))
	->column("Remove", "<a href=\"#\" onclick=\"new SurveyCreate().removeSurveyQuestion({survey_question_xref_id}); return false;\">
			<img class=\"icon\" alt=\"delete\" src=\"/fakoli/images/icon_delete_red.gif\"></a>")
;

$table->emptyMessage = "There are no questions defined.";
$table->sortable = false;
$table->cssStyle = "width: 100%";
$table->rowId = true;

$questionSelect = new DataListFieldRenderer($form, $table, "survey_questions", "Survey Questions");
$form->submitLabel = "Update Order";
$questionSelect->onPostProcess = reOrderQuestions;

if(count($xrefs) == 0)
	$form->readOnlyForm = true;

$form->button("Create a New Question", "survey_question_form?survey_id=$survey_id");

$questionSets = SurveyManager::getQuestionSets($survey_id);
if(count($questionSets))
{
	$form->button("Select from Existing Questions", "survey_question_select?survey_id=$survey_id");
}

$tabs = SurveyManager::surveyTabs($survey_id, false);

if ($method == "POST")
{
	if($form->save())
	{
		redirect("survey_questions?survey_id=$survey_id");
	}
}

$script .= $form->writeScript();

$tabs->writeHTML();

?>
<div id="tab_border">
<button id="next_page_button" class="button"
	onclick="go('<?echo $tabs->getNextPage() ?>')">Next Page &raquo;</button>
<h3><?php echo $survey->title ?></h3>
<p>Select the questions for this survey. You can select from a list of
existing questions or create new ones. Removing a question from this
survey does not delete the question from the database unless the
question is not linked to any other survey.</p>
<div id="inner_border"><?
$form->drawForm();

/*
 * If no questions, the form won't be drawn so the extra buttons
 * won't be drawn and need to be called here.
 */
if(count($xrefs) == 0)
{
	?>
<p><a class='button'
	href='survey_question_form?survey_id=<?php echo $survey_id ?>'>Create
a New Question</a>&nbsp;&nbsp; 

<?php 
	if(count($questionSets))
	{
?>
<a class='button' href="survey_question_select?survey_id=<?php echo $survey_id ?>">Select from Existing Questions</a></p>
	<?php
	}
}
?></div>
</div>
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
	$survey_id = $tableSelect->parent->data->get("survey_id");
	$survey = new Survey($survey_id);
	SurveyManager::updateSortOrder($survey);
}
?>