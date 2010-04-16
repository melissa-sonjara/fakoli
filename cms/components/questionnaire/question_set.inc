<?
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
 * Title: question_set.inc
 * 
 * Description: Handles listing and ordering the questions
 * within a questionnaire.
 *  
 * author: Janice Gallant for Sonjara, Inc.
 * 
 * date: 11/4/09
 * 
 */



/*
 * class QuestionSet - displays table list view of the set of questions
 * created for a given object.
 * 
 * This form requires the following standard field names for the table
 * that stores the set of question:
 * question_key
 * question_number
 * 
 * This form requires that the script for add/edit/delete a question
 * be in the same directory as the calling script and be in the format
 * $prefix + "_question_form.php 
 * 
 * The reorder function requires that the class that links to the questions
 * contain the relation function name "Questions".
 * 
 * Parameters:
 * $questionnaire_key
 * $readOnly - whether the questions can be edited and/or deleted
 */
class QuestionSet
{
	var $questions = array();
	var $questionnaire;
	var $key;
	var $editable;
	var $noneText = "There are not yet any questions in this questionnaire.";
	var $question_form = "question_form.php";

	function QuestionSet($questionnaire_key)
	{
		$this->questionnaire = new Questionnaire($questionnaire_key);
		$this->key = $questionnaire_key;
		$this->questions = $this->questionnaire->Questions("ORDER BY question_number");
		$this->editable= $this->questionnaire->isEditable();
	}
	
	function writeScript()
	{
		
	}
	
	function drawList()
	{
		if (count($this->questions) > 0)
		{
			if(!$this->editable)
				echo "<h5>{$this->questionnaire->getNotEditableHTML()}</h5>";
			?>
			<form style='display: inline' name="question_order_form" method="POST" action="">
			<table style="list; border: dotted 1px #000;border-collapse: collapse">
			<?
			foreach($this->questions as $question)
			{
				$this->drawOneQuestion($question);
			}
			?>
			</table>
			<br/>
			<input type="hidden" name="obj_key" value="<?echo $this->key ?>"/>
			<input class='button' type="submit" name="reorder" value="&nbsp;Update Question Order&nbsp;"/>&nbsp;&nbsp;
			<?
		} // end draw table
		else
		{
			echo $this->noneText ."<br/><br/>";
		}
		?>		
		<a href="<? echo $this->question_form ?>?questionnaire_key=<? echo $this->key ?>" class="button"> Add a Question </a><br/>
		</form>
		<?
	} // end function drawList
	
	/*
	 * drawOneQuestion - for each question row, include an edit and delete button
	 */
	function drawOneQuestion($question)
	{
		echo "<tr>";
		echo "<td style='width: 125px;border: dotted 1px #000'><b>Question</b> <input type='text' name='question_{$question->question_key}' value='{$question->question_number}' size='4'/></td>"; 
		echo "<td style='border: dotted 1px #000'>{$question->question}</td>";

		echo "<td style='border: dotted 1px #000'>&nbsp;<a class='button' href='/dgepp/admin/question_form.php?question_key={$question->question_key}'>&nbsp;Edit&nbsp;</a>&nbsp;</td>";
		if($this->editable)
		{
			echo "<td style='border: dotted 1px #000'>&nbsp;<a class='button' href='/dgepp/admin/question_delete.php?question_key={$question->question_key}&questionnaire_key={$question->questionnaire_key}' onclick=\"return confirm('Are you sure you wish to delete this question?');'\">&nbsp;Delete&nbsp;</a>&nbsp;</td>";
		}	
		echo "</tr>";
	}
	

/*
 * Reorder questions, set the question_number and save to db.
 * 
 * After implementing the user's new order, renumber the questions
 * in sequence (e.g., 1, 2, 3 rather than 1, 3, 7).
 * 
 */
	function reOrderQuestions()
	{
		global $_POST;
		
		// reorder
		foreach($_POST as $name => $value)
		{
			if (!strncmp($name, "question_", 9))
			{
				$id = substr($name, 9);
				$question = new Question();
				$question->filter = new InclusionFilter("question_key", "question_number");	
				$question->questionnaire_key = $this->key;
				$question->question_key = $id;
				$question->question_number = $value;
				$question->save();
			}
		}
		// renumber
		$questionnaire = new Questionnaire($this->key);
		$questions = $questionnaire->Questions("ORDER BY question_number");
		$value = 1;
	
		foreach($questions as $question)
		{
			$question->filter = new InclusionFilter("question_number");
			$question->question_number = $value;
			$question->save();
			$value++;
		}

	} // end reOrderQuestions
	
	
} // end class QuestionSet


