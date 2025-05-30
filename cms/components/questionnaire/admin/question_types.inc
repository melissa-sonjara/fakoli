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
 * Description: List of question types from which users can select
 * when creating/editing a question in a survey template.
 * 
 * author: Janice Gallant for Sonjara, Inc.
 * 
 * date: 8/5/10
 * 
 */

Fakoli::using("questionnaire");
Fakoli::usingFeature("data_view");

$menu_item = "Questionnaire Question Types";

$title = "Question Types";


$questionTypes = query('QuestionType', "ORDER BY sort_order");

$table = new DataListView($questionTypes, "questionTypes");
$table->column("Name", "<a href=\"/admin/question_type_form?question_type_id={question_type_id}\">{name}</a>")
		->column("Sort Order", 'formatSortOrderInputField', false, "width: 5%")
		->column("Class Name", "{class_name}")
		;
	  	
$table->emptyMessage = "There are no question types defined.";
$table->sortable = false;
$table->cssStyle = "width: 60%";


if ($method == "POST")
{
	reorderQuestionTypes();
	// refresh
	redirect("/admin/question_types");
}

$script .= $table->writeScript();

?>
<p>Defines the set of question types available for questionnaires and surveys.</p>
<form style='display: inline' name="question_type_form" method="POST" action="">

<?php
$table->drawView();
?>
<br><br><input class='button' type="submit" name="reorder" value="&nbsp;Save Order&nbsp;"/>&nbsp;&nbsp;
<a class='button' href='/admin/question_type_form'>Add a Question Type</a>
</form>
<?php 
function formatSortOrderInputField($questionType)
{
	return "<input type='text' name='question_type_{$questionType->question_type_id}' 
		value='{$questionType->sort_order}' size='4'/>"; 
	
}


function reorderQuestionTypes()
{
	global $_POST;
	// reorder
	foreach($_POST as $name => $value)
	{
		global $_POST;
		
		// reorder
		foreach($_POST as $name => $value)
		{
			if (!strncmp($name, "question_type_", 14))
			{
				$id = substr($name, 14);
				$questionType = new QuestionType();
				$questionType->filter = new InclusionFilter("question_type_id", "sort_order");	
				$questionType->question_type_id = $id;
				$questionType->sort_order = $value;
				$questionType->save();
			}
		}
		// renumber
		$questionTypes = query('QuestionType', "ORDER BY sort_order");
		$value = 1;
	
		foreach($questionTypes as $questionType)
		{
			$questionType->filter = new InclusionFilter("sort_order");
			$questionType->sort_order = $value;
			$questionType->save();
			$value++;
		}
	}
}
?>
