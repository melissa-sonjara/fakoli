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
 * question_id
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
 * $questionnaire_id
 * $readOnly - whether the questions can be edited and/or deleted
 */

Fakoli::using("questionnaire");

class QuestionSet
{
	var $mgr;
	var $questions = array();
	var $editable;
	var $buttons = array();
	var $noneText = "No questions have been added.";
	var $labelCSS = "";					///< CSS Class to use for label cells
	var $buttonCSS = "";				///< CSS Class to use for buttons
	var $componentPath ="/fakoli";
	
	function QuestionSet($mgr)
	{
		$this->mgr = $mgr;		
		$this->questions = $this->mgr->getQuestions();
		$this->editable = $this->mgr->isEditable();
	}
	
	function writeScript()
	{
		return "";
	}
	
	function drawList()
	{
		global $auto_form_defaults;
		foreach($auto_form_defaults as $field => $value)
		{
			$this->$field = $value;
		}
		
		$pk = $this->mgr->getPrimaryKey();
		
		if (count($this->questions) > 0)
		{
			
			if(!$this->editable)
				echo "<h5>{$this->mgr->getNotEditableHTML()}</h5>";
			?>
			<form style='display: inline' name="question_order_form" method="POST" action="">
			<table style="list; border: dotted 1px #000;border-collapse: collapse">
			<?				
			foreach($this->questions as $question)
			{
				$this->drawOneQuestion($question, $pk);
			}
			?>
			</table>
			<br/>
			<input type="hidden" name="obj_id" value="<?echo $this->mgr->item->$pk ?>"/>
			<input class='button' type="submit" name="reorder" value="&nbsp;Update Question Order&nbsp;"/>&nbsp;&nbsp;
			<?
		} // end draw table
		else
		{
			echo $this->noneText ."<br/><br/>";
		}
		
		$this->drawOneButton($this->mgr->getQuestionForm(), "Create a New Question");
		$this->drawButtons();
		?>
		<br/><br/>
		</form>
		<?
	} // end function drawList
	
	/*
	 * drawOneQuestion - for each question row, include an edit and delete button
	 */
	function drawOneQuestion($question, $pk)
	{
		$questionPk = $question->getPrimaryKey();
		$orderField = $this->mgr->getOrderField();
		echo "<tr>";
		echo "<td style='width: 125px;border: dotted 1px #000'>
		<b>Question</b> <input type='text' name='question_{$question->$questionPk}' value='{$question->$orderField}' size='4'/></td>"; 
		echo "<td style='border: dotted 1px #000'>{$question->question}</td>";
		
		if($question->options)
			$options = preg_replace("/\r\n/", "<br>", $question->options);
		
		echo "<td style='border: dotted 1px #000'>{$options}</td>";
		
		$disable = ($question->locked) ? "disabled='disabled'" : "";

		echo "<td style='border: dotted 1px #000'>&nbsp;";
		$this->drawOneButton($this->mgr->getQuestionForm() . $question->$questionPk, "Edit");
		echo "&nbsp;</td>";
		
		if($this->editable)
		{
			echo "<td style='border: dotted 1px #000'>&nbsp;";
			$this->mgr->drawQuestionDeleteButton($question->$questionPk);
			echo "&nbsp;</td>";
		}	
		echo "</tr>";
	}
	

/*
 * Reorder questions, set the sort_order and save to db.
 * 
 * After implementing the user's new order, renumber the questions
 * in sequence (e.g., 1, 2, 3 rather than 1, 3, 7).
 * 
 */
	function reOrderQuestions()
	{
		$this->mgr->reOrderQuestions();
	} // end reOrderQuestions
	

	/**
	 * Adds a custom button to the form.
	 *
	 * @param string $text the button label text
	 * @param string $url the URL to handle the button press
	 * @param string $confirm optional confirmation message
	 */
	function button($text, $url, $confirm = null)
	{
		$this->buttons[] = array('text' => $text, 'url' => $url, 'confirm' => $confirm);
	}
	
	/**
	 * Draws any additional buttons specified in the calling script.
	 *
	 */
	function drawButtons()
	{
		foreach($this->buttons as $button)
		{
			echo "&nbsp;&nbsp;&nbsp;&nbsp;";
			$this->drawOneButton($button['url'], $button['text'], $button['confirm']);
		}

	}
	
	static function drawOneButton($url, $text, $confirm = "", $buttonCSS = 'button')
	{
		if ($confirm)
		{
			$link = "if (confirm('".jsSafe($confirm)."')) go('{$url}'); return false;";
		}
		else
		{
			$link = "go('$url'); return false;";
		}
	
		echo "<input type='button' class='{$buttonCSS}' onclick=\"$link\" value=\"$text\"/>";
		
	}
	
	
} // end class QuestionSet


