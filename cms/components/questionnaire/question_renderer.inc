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


	function QuestionField($parent, $question)
	{
		$this->parent = $parent;
		$this->question = $question;
		$qPk = $question->getPrimaryKey();
		$this->answer = $this->parent->answers[$this->question->$qPk];
	}

	function getID()
	{
		return $this->question->get($this->question->getPrimaryKey());
	}
	
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

	function writeLimitScript()
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


	function renderLimitEnd($limit, $field, $value)
	{
		if ($limit)
		{
			echo strlen(trim($value));
			echo "</span> of {$limit} characters";
		}
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


class HeadingView extends QuestionField
{
	var $skipNumbering = true;
	
	function MultipleChoiceView($parent, $question)
	{
		$this->QuestionField($parent, $question);
	}	
	

	function drawView()
	{
		$this->writeHTML();
	}

	/*
	 * Draws the answer to a question as text, not
	 * as formatted hmtl.
	 */
	function drawSummaryView()
	{
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
		$tips = explode("\n", $this->question->context_help);
		
		$valueAnswer = $this->parent->getAnswer($this->question);

		$readonly = $this->parent->readonly ? " DISABLED" : "";

		echo "<table class=\"layout\">";

		$count = count($options);

		if($count > 0)
		{
			$idx = 1;
			foreach($options as $value => $text)
			{
				$tip = $tips[$idx - 1];
				if ($tip)
				{
					$tip = " onmouseover=\"showTextToolTip('question_{$this->question->$pk}', event, 'question_help', '".jsSafe($tip)."'); return false;\" onmouseout=\"hideToolTip('question_help');return false\" ";
				}
				
				$selected = ($valueAnswer == $idx) ? " checked" : "";

				echo "<tr><td style=\"padding:1px\"$tip><input type='radio'
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
				
		if (Settings::getValue("survey", "interactive_graphs"))
		{
			QuestionnaireResultsGraph::drawSelectResults($pk, $qPk, $this->parent->get($pk), $this->question->get($qPk));
		}
		else
		{
			$link ="/action/questionnaire/select_graph?$pk={$this->parent->get($pk)}&$qPk={$this->question->$qPk}";
			
?>
<img src="<?php echo $link ?>" alt="Pie Chart" />
<?
		}
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
		
		if (Settings::getValue("survey", "interactive_graphs"))
		{
			QuestionnaireResultsGraph::drawSelectResults($pk, $qPk, $this->parent->get($pk), $this->question->get($qPk));
		}
		else
		{
			$link ="/action/questionnaire/select_graph?$pk={$this->parent->get($pk)}&$qPk={$this->question->$qPk}";			
?>
<img src="<?php echo $link ?>" alt="Pie Chart" />
<?
		}
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


		$readonly = $this->parent->readonly ? " disabled='disabled'" : "";
		$valueAnswer = $this->parent->getAnswer($this->question);

		$qPk = $this->question->getPrimaryKey();

		$tip =  $this->question->context_help;
		if ($tip)
		{
			$tip = " onmouseover=\"showTextToolTip('question_{$this->question->$qPk}', event, 'question_help', '".jsSafe($tip)."'); return false;\" onmouseout=\"hideToolTip('question_help');return false\" ";
		}
		echo "<div$tip>$from&nbsp;";
		
		for($i = 1; $i <= $steps; ++$i)
		{
			$selected = ($valueAnswer == $i) ? " checked" : "";

			echo "<input type='radio' style='border: none' name='question_{$this->question->$qPk}'
			id='question_{$this->question->$qPk}_$i' 
			value='$i'$selected $readonly />&nbsp;";
		}

		// Save this count to loop through checkboxes on required validation
		echo "<input type=\"hidden\" name=\"count_question_{$this->question->$qPk}\" value=\"$steps\">";

		echo $to."</div>";
	}

	function getAnswer()
	{
		$this->getOptions($this->question->options, $from, $to, $steps);

		$valueAnswer = $this->parent->getAnswer($this->question);
		if($valueAnswer)
			$answer = "{$valueAnswer} rating on scale of $steps $from &ndash; $to";

		return $answer;
	}
	
	static function getOptions($options, &$from, &$to, &$steps)
	{
		list($from, $to, $steps) = explode("\n", $options);
		if (!$from) $from = "Lowest";
		if (!$to) $to = "Highest";
		if (!$steps) $steps = 5;
	}
	
	/**
	 * Draw results of all answers to one ratings question in
	 * a histogram.
	 * 
	 * Called from an action handler. Handler must include the
	 * jgraph library:
	 * 
	 * require_once ('jpgraph/jpgraph.php');
	 * require_once ('jpgraph/jpgraph_bar.php');
	 * 
	 * The handler would instantiate the survey or questionnaire, 
	 * the question, and the question renderer and retrieve the
	 * array of answers grouped by value.
	 * 
	 * For example:
	 * 
	 * $survey = new Survey($survey_id);
	 * $question = new SurveyQuestion($survey_question_id);
	 * 
	 * $answers = $survey->Answers("WHERE survey_question_id=$survey_question_id");
	 * $answers = regroupList($answers, "value");
	 * $renderer = QuestionField::create($survey, $question);
	 * $renderer->drawJGraphView($answers);
	 * 
	 * @param Array $answers - array of answers to one ratings question 
	 * grouped by field "value"
	 */
	function drawJGraphView($answers)
	{
		RatingView::drawJGraph($this->question->options, $answers);
	}
	
	/**
	 * Draw results of all answers to one ratings question in
	 * a histogram.
	 * 
	 * @param String $options - in format label for lowest\nlabel for highest\n number steps
	 * 	default options will be used if blank
	 * @param Array $answers - array of answers to one ratings question 
	 * grouped by field "value"
	 */
	static function drawJGraph($options, $answers)
	{
		$datay = array();

		$optionAnswers = array();
		$count = array();
		$optionText = array();
	
		RatingView::getOptions($options, $from, $to, $steps);
	
		for($idx = 1; $idx <= $steps; ++$idx)
		{
			if($idx > 1 AND $idx < $steps) 
				$optionText[$idx] = $idx;
			else
				$optionText[$idx] = ($idx == 1) ? $from : $to;
		}	

		$steps = count($optionText);

		for($idx = 1; $idx <= $steps; ++$idx)
		{
			$counts[$idx] = count($answers[$idx]);
			$totalValue += $idx * $counts[$idx];
			$totalCount += $counts[$idx];
			$datay[] = $counts[$idx];
			$datax[] = $optionText[$idx];
		}

		$mean = "Average: ".number_format($totalValue / $totalCount, 1);

		// Setup the graph.
		$graph = new Graph(500,300);
		$graph->img->SetMargin(60,20,35,75);
		$graph->SetScale("textint");
		$graph->SetFrame(false); // No border around the graph

		// Setup font for axis
		$graph->xaxis->SetFont(FF_VERDANA,FS_NORMAL,10);
		$graph->yaxis->SetFont(FF_VERDANA,FS_NORMAL,10);

		// Show 0 label on Y-axis (default is not to show)
		$graph->yscale->ticks->SupressZeroLabel(false);

		// Setup X-axis labels
		$graph->xaxis->SetTickLabels($datax);
		//$graph->xaxis->SetLabelAngle(50);
		$graph->xaxis->SetTitle($mean);

		// Create the bar pot
		$bplot = new BarPlot($datay);
		$bplot->SetWidth(0.6);

		// Set color for the frame of each bar
		$bplot->SetColor("black");
		$graph->Add($bplot);
		
		// Finally send the graph to the browser
		$graph->Stroke();
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

		if (Settings::getValue("survey", "interactive_graphs"))
		{
			QuestionnaireResultsGraph::drawRatingResults($pk, $qPk, $this->parent->get($pk), $this->question->get($qPk));
		}
		else
		{
			$link ="/action/questionnaire/rating_graph?$pk={$this->parent->get($pk)}&$qPk={$this->question->$qPk}";			
?>
<img src="<?php echo $link ?>" alt="Histogram" />
<?
		}
	}
}

// JDG 8/4/2011 - change to String rendering
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
		$autocomplete = "autocomplete='off'";

		$pk = $this->question->getPrimaryKey();

		$field = "question_" . $this->question->$pk;

		if ($this->question->char_limit)
		{
			$onkeypress = " onkeyup='{$field}_keyup(event);'";
		}

		$tip =  $this->question->context_help;
		if ($tip)
		{
			$tip = " onmouseover=\"showTextToolTip('question_{$this->question->$pk}', event, 'question_help', '".jsSafe($tip)."'); return false;\" onmouseout=\"hideToolTip('question_help');return false\" ";
		}		
		
		echo "<input id='$field' type='text' name='$field' $readonly $autocomplete value='".htmlspecialchars($value, ENT_QUOTES, 'UTF-8')."' size='50' $disable $onkeypress $tip/>";
		echo "<br/><div style='clear: left; float: right'><span id='{$field}_length'>";

		parent::renderLimitEnd($this->question->char_limit, $field, $value);
		echo "</div><br/>";

	}

	function drawView()
	{
		$value = $this->parent->getAnswer($this->question);
		echo "<em>".htmlsafe($value)."</em>";
	}

	function writeScript()
	{
		return parent::writeLimitScript();
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
		return parent::writeLimitScript();
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
		$autocomplete = "autocomplete='off'";
		
		$tip =  $this->question->context_help;
		if ($tip)
		{
			$tip = " onmouseover=\"showTextToolTip('question_{$this->question->$pk}', event, 'question_help', '".jsSafe($tip)."'); return false;\" onmouseout=\"hideToolTip('question_help');return false\" ";
		}
		
		echo "<textarea name='{$field}' cols='75' rows='$rows' $onkeypress $readonly $autocomplete $tip>".htmlsafe($value)."</textarea>";

		echo "<br/><div style='clear: left; float: right'><span id='{$field}_length'>";

		parent::renderLimitEnd($this->question->char_limit, $field, $value);

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
		$tips = explode("\n", $this->question->context_help);

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

				$tip = $tips[$idx - 1];
				if ($tip)
				{
					$tip = " onmouseover=\"showTextToolTip('question_{$this->question->$pk}', event, 'question_help', '".jsSafe($tip)."'); return false;\" onmouseout=\"hideToolTip('question_help');return false\" ";
				}
								
				echo "<tr><td style=\"padding:1px\"$tip><input type='checkbox'
				class='checkbox' name='question_{$this->question->$pk}[$idx]' 
				value='$value'$readonly$checked/>&nbsp;$text</td></tr>";
				$idx++;
			}

			// Save this count to loop through checkboxes on required validation
			echo "<input type=\"hidden\" name=\"count_question_{$this->question->$pk}\" value=\"$count\">";
		}

		echo "</table><br/>";

	}

	/*
	 * Indicate count of required checkboxes
	 * 
	 * If the count of options is 1 and the
	 * question is required, then just
	 * show "*" to denote required.
	 */
	function getRequired()
	{
		if($this->question->required > 0)
		{
			$optionArr = explode("\n", $this->question->options);
			if(count($optionArr) > 1)
			{
				$label = ($this->question->required > 1) ? "checkboxes" : "checkbox";
				$required = "* at least " . $this->question->required . " $label must be checked";
			}
			else
				$required = "*";
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
		$pk = $this->parent->primary_key;
		$qPk = $this->question->getPrimaryKey();
				
		if (Settings::getValue("survey", "interactive_graphs"))
		{
			QuestionnaireResultsGraph::drawChecklistResults($pk, $qPk, $this->parent->get($pk), $this->question->get($qPk));
		}
		else
		{
			$link ="/action/questionnaire/checklist_graph?$pk={$this->parent->get($pk)}&$qPk={$this->question->$qPk}";
?>
<img src="<?php echo $link ?>" alt="Histogram" />
<?
		}
	}
}
