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
 * Title: questionnaire_form.inc
 *
 * Description: Class for building custom surveys, questionaires,
 * quizzes and response interface.
 *
 * The form formats the question display based on the
 * type of question.
 *
 * The specifics of the different classes that can render
 * questions are handled by a manger class which provides
 * information such as the name of the question_id field,
 * gets the set of Questions and Answers through its relations,
 * and saves to its specific type of data structure.
 *
 * author: Janice Gallant for Sonjara, Inc.
 *
 * date: 11/4/09
 *
 */

Fakoli::using("questionnaire");


/*
 * QuestionnaireForm
 *
 * @param: $mgr - an object of class QuestionnnaireManager, SurveyManager,
 * or other which provides specific data and values to this form as
 * requested.
 * @param: $readonly - defaults to false - whether questionnaire is
 * being reviewed or answered.
 */

class QuestionnaireForm
{
	var $mgr; // the object that links to the questions and answers: questionnaire, survey, quiz, etc.
	var $questions;
	var $answers;
	var $readonly;
	var $submitLabel = "Save";
	var $validators = array();
	var $requiredText = "<b>You must answer all required questions.</b><br/>";
	var $msg; // validation msg
	var $num = 1; // starting number for first question; may be set by calling script to
	// append multiple question sets for review.
	var $onSaveComplete = null;	///< Callback event handler that is fired after the AutoForm has finished saving data to the database.

	// AutoForm defaults
	var $labelCSS = "";					///< CSS Class to use for label cells
	var $valueCSS = "";					///< CSS Class to use for field/value cells
	var $buttonCSS = "";				///< CSS Class to use for buttons
	var $inputCSS = "";					///< CSS Class to use for input fields
	var $checkboxCSS = "";				///< CSS Class to use for checkbox fields
	var $markRequiredFields = false;	///< Set to true to add an asterisk after required field labels.
	var $onFormatLabel = null; 			///< Label Processing hook
	var $requiredFieldsText = "* indicates required question";
	var $subordinate = false;
	var $buttons = array();				///< The custom buttons collection
	var $allowIncompleteSave = true;	///< Allows users to save partially completed forms
	
	function QuestionnaireForm($mgr, $readonly = false)
	{
		global $auto_form_defaults;

		$this->mgr = $mgr;
		$this->readonly = $readonly;

		foreach($auto_form_defaults as $field => $value)
		{
			$this->$field = $value;
		}

		$questions = $this->mgr->getQuestions();
			
		if(count($questions) > 0)
		{
			$this->answers = $this->getIndexedAnswers();

			foreach($questions as $question)
			{
				$this->questions[] = QuestionField::create($this, $question);
			}
				
			foreach($this->questions as $question)
			{
				if($question->question->required)
					array_push($this->validators, $question->getRequiredValidator());
			}
		}
	}

	function makeSubordinate($subordinate = true)
	{
		$this->subordinate = $subordinate;
	}

	function getValidationEngine()
	{
		return $this;
	}
			
	/**
	 * Adds a custom button to the form.
	 *
	 * @param string $text the button label text
	 * @param string $url the URL to handle the button press
	 * @param string $confirm optional confirmation message
	 * @param boolean $isScript true if the url is javascript code to execute, 
	 * 							false if it is a URL to redirect to
	 */
	function button($text, $url, $confirm = null, $isScript = false)
	{
		$this->buttons[] = array('text' => $text, 'url' => $url, 'confirm' => $confirm, 'isScript' => $isScript);
	}
	
	/**
	 * Draws any additional buttons specified in the calling script.
	 *
	 */
	function drawButtons()
	{
		foreach($this->buttons as $button)
		{
			$url = ($button['isScript']) ? $button['url'] : "go('{$button['url']}');";
			
			if ($button['confirm'])
			{
				$link = "if (confirm('".jsSafe($button['confirm'])."')) $url; return false;";
			}
			else
			{
				$link = "$url; return false;";
			}

			echo "&nbsp;&nbsp;&nbsp;&nbsp;<input type='button' class='{$this->buttonCSS}' onclick=\"$link\" value=\"{$button['text']}\"/>";
		}

	}
	function getAnswer($question)
	{
		$qPk = $this->mgr->getQuestionKey();
		$answer = $this->answers[$question->$qPk];
		return ($answer) ? $answer->value : "";
	}

	function writeScript()
	{
		$script = "";

		if(count($this->questions) == 0)
			return;
			
		foreach($this->questions as $question)
		{
			$script .= $question->writeScript();
		}


		$script .= "function validate_Questionnaire_form(form)\n";
		$script .= "{";

		foreach($this->validators as $validator)
			$script .= $validator->writeClient();

		$script .= "\n\n return true;\n}";
		$script .= "\n\n";
		$script .= "function onSubmitQuestionnaire_form(form)\n{\n";

		if ($this->allowSaveIncomplete)
		{
			// return true because we need to save their answers, even if they
			// didn't answer all questions (invalid form)
			$script .= "\t validate_Questionnaire_form(form); return true\n}";
		}
		else
		{
			$script .= "\t return validate_Questionnaire_form(form);\n}";
		}
		if ($script) $script = "<script type='text/javascript'>\n$script\n</script>";
		return $script;
	}

	function writeHTML($displayOnly = false)
	{
		if(count($this->questions) == 0)
		{
			echo "There are no questions.<br/>";
			return;
		}

		if($this->requiredText AND !$displayOnly)
		{
			if ($this->msg)
			{
				echo "<tr>\n <td colspan='2'><span class='error'>{$this->msg}</span</td></tr>\n";
				$this->msg = "";
				echo "</span>\n";
			}
			echo $this->requiredText;
		}

		if(!$displayOnly)
			echo "<b>{$this->requiredFieldsText}</b><br/><br/>";

		foreach($this->questions as $question)
		{
			$this->writeOneQuestion($question, $displayOnly);
			$this->num++;
		}

		echo "</dl>";
	}

	function writeOneQuestion($question, $displayOnly)
	{
		$required = ($displayOnly) ? "" : $question->getRequired();
		echo " <dt><label for='question_{$this->num}>'<b>{$this->num}.</b> {$question->question->question}{$required}</label></dt>\n";

		echo " <dd>";
		if(!$displayOnly)
			$question->writeHTML();
		else
			$question->drawView();

		echo " </dd><br/>";

		if($displayOnly)
			echo "<br/>";
		echo "</dl>";
	}


	function drawForm()
	{
		echo "<div class='questionnaire_form'>";

		if (!$this->subordinate)
		{
			echo "<form id=\"Questionnaire_form\" method=\"POST\" action=\"\" ";
			echo "enctype='multipart/form-data' onsubmit='return onSubmitQuestionnaire_form(this);'>";
		}

		$this->writeHTML();

		if (!$this->subordinate)
		{
			echo "<br/>";
			echo "<input type=\"submit\" style=\"float: left\" name=\"save\" class=\"button\" value=\"&nbsp;&nbsp;{$this->submitLabel}&nbsp;&nbsp;\"/>";
			$this->drawButtons();
			echo "<br/>";
			echo "</form>";
		}

		echo "</div>";
	}


	// display or preview view
	function drawView()
	{
		$this->writeHTML(true);
	}

	function getIndexedAnswers()
	{
		$answers = $this->mgr->getAnswers();

		if(count($answers) > 0)
			$answers = reindexList($answers, $this->mgr->getQuestionKey());
			
		return $answers;
	}

	function validate()
	{
		foreach($this->validators as $validator)
		{
			$msg = $validator->validate();
			if ($msg)
			{
				$result .= $msg."<br>";
			}
		}
		$this->msg = $msg;
		return $result;
	}


	function save()
	{
		global $_POST;

		$answers = $this->getIndexedAnswers();
		$pk = $this->mgr->item->getPrimaryKey();
		$qPk = $this->mgr->getQuestionKey();
		$aPk = $this->mgr->getAnswerKey();

		foreach($_POST as $name => $value)
		{
			// convert array of checks to comma delimated string
			if (is_array($value))
			{
				$valueAnswer = implode(",",  array_values($value));
			}
			else
			{
				$valueAnswer = $value;
			}
				
				
			if (!strncmp("question_", $name, 9))
			{
				$question_id = substr($name, 9);

				$answer = $answers[$question_id];
				$answerClass = $this->mgr->getAnswerClass();

				if(!$answer)
				{
					$answer = new $answerClass();
					trace("QuestionnaireForm::save pk is $pk qPk is $qPk question_id is $question_id", 3);
					// e.g., response_id
					$answer->$pk = $this->mgr->item->$pk;
					$answer->$qPk = $question_id;
					$answer->value = $valueAnswer;
					$answer->save();
				}
				else
				{
					$answer = new $answerClass($answer->$aPk);
					$answer->filter = new InclusionFilter("value");
					$answer->value = $valueAnswer;
					$answer->save();
				}
			}
		}

		// Need to handle deleted checkbox answers which don't appear in $_POST
		$questions = $this->mgr->getQuestions();

		if(count($questions) > 0)
		{
			$indexed = reindexList($questions, "question_type_id");
			$checkListType = querySingle(QuestionType, "WHERE class_name = 'CheckListView'");
			$checkListQuestions = $indexed[$checkListType->question_type_id];
				
			if(count($checkListQuestions) < 0)
			{
				foreach($checkListQuestions as $question)
				{
					$answer = $answers[$question->$qPk];
					if($answer AND $answer->value)
					{
						$postAnswer = $_POST["question_{$question->question_number}"];
						if(!$postAnswer AND !is_array($postAnswer))
						{
							$answer->filter = new InclusionFilter("value");
							$answer->value = "";
							$answer->save();
						}
					}
				}
			}
		}


		// Even if invalid, we still want to save their answers
		$this->msg = $this->validate();
		if ($this->msg != "") return false;


		// onComplete event is fired once all processing has been completed
		if ($this->onSaveComplete)
		{
			$onComplete = $this->onSaveComplete;
			$onComplete($this);
		}

		return true;

	}


	// change from starting at 0 to starting at 1
	static function incrementArray($array)
	{
		if(count($array) > 0)
		{
			$idx = 0;
			while($idx < (count($array)))
			{
				$newArray[$idx+1] = $array[$idx];
				$idx++;
			}
		}
		return $newArray;
	}
}



/*
 * For questionnaire creator to view the layout of the questionnaire
 * from the questionnaire data entry interface.
 */
class QuestionnairePreview extends QuestionnaireForm
{
	var $mgr;

	function QuestionnairePreview($mgr)
	{
		$this->mgr = $mgr;
		$this->readonly = true;

		$questions = $this->mgr->getQuestions();

		if(count($questions) == 0) return;

		foreach($questions as $question)
		{
			$this->questions[] = QuestionField::create($this, $question);
		}
	}

	function writeScript()
	{
		return "";
	}
}


/*
 * QuestionnaireResponseView
 *
 * @param: $mgr - an object of class QuestionnnaireManager, SurveyManager,
 * or other which provides specific data and values to this form as
 * requested.
 * 
 * This view is similar to calling QuestionnaireForm as readonly
 * except that responses are displayed in summary or text only form,
 * not as disabled fields readonly fields.
 * 
 * Calling page should call function drawView rather than
 * drawForm for display only view.
 * 
 * e.g.,
 * 
 * $mgr = new MyQuestionnaireResponseManger($myObj);
 * $view = new QuestionnaireResponseView($mgr);
 * 
 * $view->drawView();
 */

class QuestionnaireResponseView extends QuestionnaireForm
{
	function QuestionnaireResponseView($mgr)
	{
		parent::QuestionnaireForm($mgr, true);
	}

	function writeOneQuestion($question, $displayOnly = false)
	{
		$required = $question->getRequired();
		echo " <dt><b>{$this->num}.</b> {$question->question->question}{$required}</dt>\n";
		echo " <dd>";
		$question->drawSummaryView();
		echo " </dd><br/>";
		echo "</dl>";
	}
}
?>