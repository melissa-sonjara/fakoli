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
 * Title: question_renderer.inc
 * 
 * Description: Classes for rendering different types of questions:
 *  multiple choice
 *  rating
 *  short text
 *  free text
 *  checklist 
 * 
 * author: Janice Gallant for Sonjara, Inc.
 * 
 * date: 11/4/09
 */ 

Fakoli::using("questionnaire");
require_once realpath(dirname(__FILE__)."/validation.inc");

/*
 * QuestionField
 * 
 * Displays each type of question
 */
class QuestionField
{
	var $parent;
	var $question;
	var $readonly;
	var $answer;
	var $answerSeparator = "<br>"; // for list of answers in checklist renderer
	
	function writeScript()
	{
		return "";
	}
	
	function writeHTML()
	{
		return "";
	}
	
	function writeReportHTML()
	{
	}
	
	/*
	 * Draws the answer to a question as text, not
	 * as formatted hmtl.
	 */
	function drawSummaryView()
	{
		echo "<p>" . $this->getAnswer() . "</p>";
	}
	
	/*
	 * Gets the answer, unmassaged, from the value
	 * field of the table. Used for freetext and
	 * shorttext.
	 */
	function getAnswer()
	{
		return $this->parent->getAnswer($this->question);
	}
	
	// Get single answer from a set of options
	// (SelectField and Multichoice)
	function getOptionAnswer()
	{
		$pk = $this->question->getPrimaryKey();
		$options = explode("\n", $this->question->options);
		
		$valueAnswer = $this->parent->getAnswer($this->question);
		
		$count = count($options);	
		if($count > 0)
		{
			$idx = 1;
			foreach($options as $value => $text)
			{
				if($valueAnswer == $idx)
				{
					$answer = $text;
					break;
				}
				$idx++;
			}
		}	

		return $answer;	
	}
		
	function QuestionField($parent, $question)
	{
		$this->parent = $parent;
		$this->question = $question;
		$qPk = $question->getPrimaryKey();
		$this->answer = $this->parent->answers[$this->question->$qPk];
	}
	
	// Indicate required with asterisk
	function getRequired()
	{
		return ($this->question->required) ? "*" : "";
	}
	
	function getRequiredValidator()
	{
		// empty answer signals to validate from form and $_POST
		$pk = $this->question->getPrimaryKey();
		
		$answer = ($this->parent->readonly) ? $this->answer : "";
		$question_number = $this->question->hasField("question_number") ? $this->question->question_number : $this->parent->mgr->getQuestionNumber($this->question);
		return new RequiredQuestionValidator("question_{$this->question->$pk}", "Question {$question_number}", "", $answer);
	}
	
	
	static function create($parent, $question)
	{
		$questionType = new QuestionType($question->question_type_id);
		
		if($questionType AND $questionType->class_name)
			return new $questionType->class_name($parent, $question);
		else
		{
			trace("QuestionField::create(): Unknown question type", 2);
			die("Unknown question type");
		}
	}
}




/*
 * readonly attribute doesn't work for radio buttons;
 * use "disabled" instead.
 * 
 * Radio buttons are in a table layout so that they align properly in IE7
 */
class MultipleChoiceView extends QuestionField
{
	function MultipleChoiceView($parent, $question)
	{
		$this->QuestionField($parent, $question);
	}
	
	
	function writeHTML()
	{
		$pk = $this->question->getPrimaryKey();
		$options = explode("\n", $this->question->options);

		$valueAnswer = $this->parent->getAnswer($this->question);

		$readonly = $this->parent->readonly ? " DISABLED" : "";
		
		echo "<table class=\"layout\">";
		
		$count = count($options);
		
		if($count > 0)
		{
			$idx = 1;
			foreach($options as $value => $text)
			{
				$selected = ($valueAnswer == $idx) ? " checked" : "";

				echo "<tr><td style=\"padding:1px\"><input type='radio' 
				style='border: none' name='question_{$this->question->$pk}' id='question_{$this->question->$pk}_$idx' 
				value='$idx'$selected $readonly>&nbsp;$text</td></tr>";
				$idx++;
			}
		}
		
		// Save this count to loop through checkboxes on required validation
		echo "<input type=\"hidden\" name=\"count_question_{$this->question->$pk}\" value=\"$count\">";		
		
		echo "</table>";
		
	}
	
	function drawView()
	{
		$this->writeHTML();	
	}
		
	/*
	 * Get the single text answer for this mult choice question
	 */
	function getAnswer()
	{
		return $this->getOptionAnswer();
	}
	
	function getRequiredValidator()
	{
		$pk = $this->question->getPrimaryKey();
			// Empty question object signals to validator to validate from form and $_POST.
		// Not empty means this is a view so we have the answer object that was saved.
		$answer = ($this->parent->readonly) ? $this->answer : "";
		$question_number = $this->question->hasField("question_number") ? $this->question->question_number : $this->parent->mgr->getQuestionNumber($this->question);	
		return new RequiredRadioButtonQuestionValidator("question_{$this->question->$pk}", "Question {$question_number}", $answer);
	}
	
	function writeReportHTML()
	{
		$pk = $this->parent->primary_key;
		$qPk = $this->question->getPrimaryKey();
		$link ="/action/questionnaire/select_graph?$pk={$this->parent->get($pk)}&$qPk={$this->question->$qPk}";
							
?>
<img src="<?php echo $link ?>" alt="Histogram"/>
<?
	}	
}




/*
 * Creates a drop down view of options list
 * 
 */
class SelectFieldView extends QuestionField
{
	function SelectFieldView($parent, $question)
	{
		$this->QuestionField($parent, $question);
	}
	
	
	function writeHTML()
	{
		$options = explode("\n", $this->question->options);

		$num = 1;
		$value = $this->parent->getAnswer($this->question);

		$readonly = $this->parent->readonly ? " DISABLED" : "";
		
		if(count($options) > 0)
		{
			echo "<table class=\"layout\">";
			$pk = $this->question->getPrimaryKey();
			echo "<select name='question_{$this->question->$pk}'>\n";	
			if(!$readonly)	
				echo "<option value=''></option>"; // blank
			
			foreach($options as $option)
			{
				$selected = ($value == $num) ? " selected" : "";				
				echo "<option value='$num'$readonly$selected>$option</option>";
				$num++;
			}
			
			echo "</select>";
		}
		
		echo "</table>";
	}
	
	function drawView()
	{
		$this->writeHTML();	
	}
	
	
	function getAnswer()
	{
		return $this->getOptionAnswer();
	}
	
	function writeReportHTML()
	{
		$pk = $this->parent->primary_key;
		$qPk = $this->question->getPrimaryKey();
		$link ="/action/questionnaire/select_graph?$pk={$this->parent->get($pk)}&$qPk={$this->question->$qPk}";
							
?><img src="<?php echo $link ?>" alt="Histogram"/><?
	}	
}

class RatingView extends QuestionField
{
	function RatingView($parent, $question)
	{
		$this->QuestionField($parent, $question);
	}
	
	function writeHTML()
	{
		list($from, $to, $steps) = explode("\n", $this->question->options);
		if (!$from) $from = "Lowest";
		if (!$to) $to = "Highest";
		if (!$steps) $steps = 5;
		
		echo "$from&nbsp;";
		
		$readonly = $this->parent->readonly ? " disabled='disabled'" : "";
		$valueAnswer = $this->parent->getAnswer($this->question);
		
		$qPk = $this->question->getPrimaryKey();
		
		for($i = 1; $i <= $steps; ++$i)
		{
			$selected = ($valueAnswer == $i) ? " checked" : "";

			echo "<input type='radio' style='border: none' name='question_{$this->question->$qPk}' 
			id='question_{$this->question->$qPk}_$i' 
			value='$i'$selected $readonly />&nbsp;";
		}
		
		// Save this count to loop through checkboxes on required validation
		echo "<input type=\"hidden\" name=\"count_question_{$this->question->$qPk}\" value=\"$steps\">";		
		
		echo $to;
	}
	
	function getAnswer()
	{
		list($from, $to, $steps) = explode("\n", $this->question->options);
		if (!$from) $from = "Lowest";
		if (!$to) $to = "Highest";
		if (!$steps) $steps = 5;
		
		$answer = "$from&nbsp;";
		
		$valueAnswer = $this->parent->getAnswer($this->question);		
		$qPk = $this->question->getPrimaryKey();
		
		for($i = 1; $i <= $steps; ++$i)
		{
			$selected = ($valueAnswer == $i) ? " checked" : "";

			$answer .= "<input type='radio' style='border: none' name='question_{$this->question->$qPk}' 
			id='question_{$this->question->$qPk}_$i' 
			value='$i'$selected disabled='disabled' />&nbsp;";
		}
		
		$answer .= $to;

		return $answer;
	}
	
	function drawView()
	{
		$this->writeHTML();	
	}

	function getRequiredValidator()
	{
		// Empty question object signals to validator to validate from form and $_POST.
		// Not empty means this is a view so we have the answer object that was saved.
		$pk = $this->question->getPrimaryKey();	
		$answer = ($this->parent->readonly) ? $this->answer : "";
		$question_number = $this->question->hasField("question_number") ? $this->question->question_number : $this->parent->mgr->getQuestionNumber($this->question);	
		return new RequiredRadioButtonQuestionValidator("question_{$this->question->$pk}", "Question {$question_number}", $answer);
	}
	
	function writeReportHTML()
	{
		$pk = $this->parent->primary_key;
		$qPk = $this->question->getPrimaryKey();
		$link ="/action/questionnaire/rating_graph?$pk={$this->parent->get($pk)}&$qPk={$this->question->$qPk}";
							
?><img src="<?php echo $link ?>" alt="Histogram"/><?
	}
}

class ShortTextView extends QuestionField
{
	function ShortTextView($parent, $question)
	{
		$this->QuestionField($parent, $question);
	}
	
	function writeHTML()
	{
		$value = $this->parent->getAnswer($this->question);
		$readonly = $this->parent->readonly ? " readonly='readonly'": "";
		$pk = $this->question->getPrimaryKey();
	
		echo "<textarea name='question_{$this->question->$pk}' cols='75' rows='1' $readonly>".htmlsafe($value)."</textarea>";
	}
	
	function drawView()
	{
		$value = $this->parent->getAnswer($this->question);
		echo "<em>".htmlsafe($value)."</em>";
	}
	
	function writeReportHTML()
	{
		$pk = $this->question->getPrimaryKey();	
		$answers = $this->parent->Answers("WHERE $pk={$this->question->$pk}");
?>
<ul>
<?
		foreach($answers as $answer)
		{
			if ($answer->value)
			{
?>
	<li><?echo formatAsHTML(stripHTML($answer->value)) ?></li>
<?
			}
		}
?>
</ul>
<?
	}
	
}

class FreeTextView extends QuestionField
{

	function FreeTextView($parent, $question)
	{
		$this->QuestionField($parent, $question);
	}
	
	function writeScript()
	{
		if ($this->question->char_limit)
		{

			$pk = $this->question->getPrimaryKey();
			$field = "question_{$this->question->$pk}";
					
		$script = <<<ENDSCRIPT
						
/* <![CDATA[ */
	function {$field}_keyup(event)
	{
		var form = document.getElementById('Questionnaire_form');
		var ctrl = form['{$field}'];
		var len = ctrl.value.length;
		if (len >= {$this->question->char_limit}) 
		{
			ctrl.value = ctrl.value.substring(0, {$this->question->char_limit});
			len = {$this->question->char_limit};
		}

		var count = document.getElementById('{$field}_length');
		count.innerHTML = len;
	}

ENDSCRIPT;
	
			return $script;

		}
	}
	
	
	function writeHTML()
	{
		$pk = $this->question->getPrimaryKey();
		$field = "question_{$this->question->$pk}";
		
		$rows = ($this->question->num_rows) ? $this->question->num_rows : 6;
		
		echo "<div>";
		if ($this->question->char_limit)
		{
			$onkeypress = " onkeyup='{$field}_keyup(event);'";
		}
		
		$value = $this->parent->getAnswer($this->question);
		
		$readonly = $this->parent->readonly ? " readonly='readonly'": "";
		echo "<textarea name='{$field}' cols='75' rows='$rows' $onkeypress $readonly>".htmlsafe($value)."</textarea>";
		
				
		echo "<br/><div style='clear: left; float: right'><span id='{$field}_length'>";
		if ($this->question->char_limit)
		{
			echo strlen(trim($value));
			echo "</span> of {$this->question->char_limit} characters";
		}
		echo "</div></div><br/>";
		
	}
	
	function drawView()
	{
		$value = $this->parent->getAnswer($this->question);
			
		/* JDG 2/9/10, htmlsafe makes some text with bad characters
		 * disappear 
		 */
		
		$answerVal = htmlsafe($value);
		if(!$answerVal)
			$answerVal = formatAsHTML($value);
		
		echo "<em>".$answerVal."</em>";	
	}
	
	
		
	function writeReportHTML()
	{
		$pk = $this->question->getPrimaryKey();	
		$answers = $this->parent->Answers("WHERE $pk={$this->question->$pk}");
?>
<ul>
<?
		foreach($answers as $answer)
		{
			if ($answer->value)
			{
?>
	<li><?echo formatAsHTML(stripHTML($answer->value))?></li>
<?
			}
		}
?>
</ul>
<?
	}
	
}


class CheckListView extends QuestionField
{
	function CheckListView($parent, $question)
	{
		$this->QuestionField($parent, $question);
	}
	
	function getValues()
	{
		$values = $this->parent->getAnswer($this->question);
		
		// include 0 value, first checkbox
		if(strlen($values)==1 AND is_numeric($values))
			$values = array($values);
		elseif(strlen($values)==0)
			$values = "";
		else
			$values = explode(",", $values);
		
		return $values;
	}
	
	function writeHTML()
	{
		$optionArr = explode("\n", $this->question->options);
		$options = $this->parent->incrementArray($optionArr);
	
		$values = $this->getValues();
		
		$readonly = $this->parent->readonly ? " DISABLED" : "";
		
		// to do - put the font style into css
		echo "<table class=\"layout\ style=\"color: #003366; font-style: normal; font-size:9; font-family: Arial,Geneva,sans-serif;\">";
		$idx = 1;
		
		$count = count($options);
		$pk = $this->question->getPrimaryKey();
		
		if($count > 0)
		{
			foreach($options as $value => $text)
			{
				if(is_array($values))
				{				
					$checked = (array_search($value, $values) !== FALSE) ? " checked" : "";
				}
								
				echo "<tr><td style=\"padding:1px\"><input type='checkbox' 
				class='checkbox' name='question_{$this->question->$pk}[$idx]' 
				value='$value'$readonly$checked/>&nbsp;$text</td></tr>";
				$idx++;
			}

			// Save this count to loop through checkboxes on required validation
			echo "<input type=\"hidden\" name=\"count_question_{$this->question->$pk}\" value=\"$count\">";		
		}
		
		echo "</table><br/>";
		
	}
	
	// Indicate count of required checkboxes
	function getRequired()
	{
		if($this->question->required > 0)
		{
			$required = "* at least " . $this->question->required;
			$required .= ($this->question->required > 1) ? " checkboxes must be checked" : " checkbox must be checked";
		} 
		return $required;
	}
	
	function getRequiredValidator()
	{
		// Empty question object signals to validator to validate from form and $_POST.
		// Not empty means this is a view so we have the answer object that was saved.
		$pk = $this->question->getPrimaryKey();
		$answer = ($this->parent->readonly) ? $this->answer : "";
		$question_number = $this->question->hasField("question_number") ? $this->question->question_number : $this->parent->mgr->getQuestionNumber($this->question);
		return new RequiredCheckListQuestionValidator("question_{$this->question->$pk}", "Question {$question_number}", $this->question->required, $answer);
	}
	
	function drawView()
	{
		$this->writeHTML();
	}

		
	function getAnswer()
	{
		$optionArr = explode("\n", $this->question->options);
		$options = $this->parent->incrementArray($optionArr);
		
		$answers = array();
		$values = $this->getValues();
				
		$count = count($options);	
		if($count > 0)
		{
			$idx = 1;
			foreach($options as $value => $text)
			{
				if(is_array($values))
				{				
					if(array_search($value, $values) !== FALSE)
					{
						$answers[] = $text;				
					}				
				}
				$idx++;
			}
		}		
		return implode($this->answerSeparator, $answers);
	}
	
	function writeReportHTML()
	{
		//$view = new CheckListResultsGraph(new SurveyManager($this->parent->survey), $this->question);
		//$view->drawView();
	
		$pk = $this->parent->primary_key;
		$qPk = $this->question->getPrimaryKey();
		$link ="/action/questionnaire/checklist_graph?$pk={$this->parent->get($pk)}&$qPk={$this->question->$qPk}";
		
		//require_once $link;
?>
<img src="<?php echo $link ?>" alt="Histogram"/>
<?

	}	
}
