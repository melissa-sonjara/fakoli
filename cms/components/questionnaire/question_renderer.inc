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

require_once realpath(dirname(__FILE__)."/../../framework/validation.inc");
require_once realpath(dirname(__FILE__)."/../../components/questionnaire/validation.inc");


// Question Options
define("QuestionMultipleChoice", 1);
define("QuestionRating", 2);
define("QuestionShortText", 3);
define("QuestionFreeText", 4);
define("QuestionCheckList", 5);

$questionTypes = array
(
	QuestionFreeText		=>	"Free Text",
	QuestionShortText		=>	"Short Text",
	QuestionCheckList		=>	"CheckList",	
	QuestionMultipleChoice 	=> 	"Multiple Choice",
	QuestionRating			=> 	"Rating"
	);




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
	
	function writeScript()
	{
		return "";
	}
	
	function writeHTML()
	{
		return "";
	}
	
	function QuestionField($parent, $question)
	{
		$this->parent = $parent;
		$this->question = $question;
		$this->answer = $this->parent->answers[$this->question->question_key];
		
	}
	
	// Indicate required with asterisk
	function getRequired()
	{
		return ($this->question->required) ? "*" : "";
	}
	
	function getRequiredValidator()
	{
		// empty answer signals to validate from form and $_POST
		$answer = ($this->parent->readonly) ? $this->answer : "";
		return new RequiredQuestionValidator("question_{$this->question->question_key}", "Question {$this->question->question_number}", "", $answer);
	}
	
	
	static function create($parent, $question)
	{
		switch($question->question_type)
		{
		case QuestionMultipleChoice:
			
			return new MultiChoiceView($parent, $question);
				
		case QuestionRating:

			return new RatingView($parent, $question);
		
		case QuestionShortText:

			return new ShortTextView($parent, $question);
			
		case QuestionFreeText:

			return new FreeTextView($parent, $question);
			
		case QuestionCheckList:
		
			return new CheckListView($parent, $question);
			
				
			default:
			
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
class MultiChoiceView extends QuestionField
{
	function MultiChoiceView($parent, $question)
	{
		$this->QuestionField($parent, $question);
	}
	
	
	function writeHTML()
	{
		$options = explode("\n", $this->question->options);

		$num = 1;
		$value = $this->parent->getAnswer($this->question);

		$readonly = $this->parent->readonly ? " DISABLED" : "";
		
		echo "<table class=\"layout\">";
		
		foreach($options as $option)
		{
			$checked = ($value == $num) ? " checked='checked'" : "";
			
			echo "<tr><td style=\"padding:1px\"><input type='radio' style=\"border:none\" name='question_{$this->question->question_key}' value='$num'$readonly$checked/>&nbsp;{$option}</td></tr>";	
			
			$num++;
		}
		
		echo "</table>";
	}
	
	function drawView()
	{
		$this->writeHTML();	
	}
	
}

class RatingView extends QuestionField
{
	function QuestionRating($parent, $question)
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
		$readonly = $this->parent->readonly ? " readonly='readonly'": "";
		$value = $this->parent->getAnswer($this->question);
		
		
		for($i = 1; $i <= $steps; ++$i)
		{
			$checked = ($value == $i) ? " checked='checked'" : "";
			echo "<input type='radio' name='question_{$this->question->question_key}' value='$i'$readonly$checked/>&nbsp;";
		}
		
		echo $to;
	}
	
		
	function drawView()
	{
		$this->writeHTML();	
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
		echo "<textarea name='question_{$this->question->question_key}' cols='80' $readonly>".htmlsafe($value)."</textarea>";
	}
	
	function drawView()
	{
		$value = $this->parent->getAnswer($this->question);
		echo "<em>".htmlsafe($value)."</em>";
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

			$field = "question_{$this->question->question_key}";
					
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
		$field = "question_{$this->question->question_key}";
		
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
	
}


class CheckListView extends QuestionField
{
	function CheckListView($parent, $question)
	{
		$this->QuestionField($parent, $question);
	}
	
	function writeHTML()
	{
		$optionArr = explode("\n", $this->question->options);

		$options = $this->parent->incrementArray($optionArr);
	
		$values = $this->parent->getAnswer($this->question);
		
		// include 0 value, first checkbox
		if(strlen($values)==1 AND is_numeric($values))
			$values = array($values);
		elseif(strlen($values)==0)
			$values = "";
		else
			$values = explode(",", $values);
			
		$readonly = $this->parent->readonly ? " DISABLED" : "";
		
		// to do - put the font style into css
		echo "<table class=\"layout\ style=\"color: #003366; font-style: normal; font-size:9; font-family: Arial,Geneva,sans-serif;\">";
		$idx = 1;
		
		$count = count($options);
		
		if($count > 0)
		{
			foreach($options as $value => $text)
			{
				if(is_array($values))
				{				
					$checked = (array_search($value, $values) !== FALSE) ? " checked" : "";
				}
								
				echo "<tr><td style=\"padding:1px\"><input type='checkbox' 
				class='checkbox' name='question_{$this->question->question_key}[$idx]' 
				value='$value'$readonly$checked/>&nbsp;$text</td></tr>";
				$idx++;
			}

			// Save this count to loop through checkboxes on required validation
			echo "<input type=\"hidden\" name=\"count_question_{$this->question->question_key}\" value=\"$count\">";		
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
		$answer = ($this->parent->readonly) ? $this->answer : "";
		return new RequiredCheckListQuestionValidator("question_{$this->question->question_key}", "Question {$this->question->question_number}", $this->question->required, $answer);
	}
	
	function drawView()
	{
		$this->writeHTML();
	}
}
