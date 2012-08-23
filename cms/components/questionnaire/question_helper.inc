<?php
/**************************************************************

 Copyright (c) 2010 Sonjara, Inc

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

class QuestionTableHelper
{
	function QuestionTableHelper()
	{
		
	}
	
	/**
	 * 
	 * @param $obj - instance of a survey question xref class or question class
	 */
	function formatOptions($obj)
	{
		if(!$obj->hasField("question_type_id") && $obj->hasRelation(Question))
		{
			$question = $obj->Question();
			if (!$question) return "";
		}
		else
		{
			$question = $obj;
		}
		
		if($question->isFreeText() || $question->isShortText())
		{
			$text .= "Character Limit: ";
			$text .=  ($question->char_limit) ? $question->char_limit : "None";
			$text .= "<br>";
		}
		else
		{
			$text =  preg_replace("/\s+\n/", "; ", $question->options);
		}

		if($question->isFreeText())
			$text .= "Number of Rows: " . $question->num_rows . "<br>";
			
		return $text;
	}


	/**
	 * 
	 * @param $obj - instance of a survey question xref class or question class
	 */
	function formatRequired($obj)
	{
		if(!$obj->hasField("question_type_id") && $obj->hasRelation(Question))
		{
			$question = $obj->Question();
			if (!$question) return "";
		}
		else
		{
			$question = $obj;
		}
	
		if($question->isCheckList())
			return ($question->required > 0) ? $question->required : "No";
		else
			return ($question->required == 1) ? "Yes" : "No";
	}

	static function formatQuestionNameInputField($question)
	{
		if(!$question->question_name)
			$question->question_name = substr($question->question, 0, 79);

		$pk = $question->getPrimaryKey();
		return $question->format("<input type='text' name=\"question_{$pk}\"
		value=\"{question_name}\" size=\"40\"/>");	
	}
}


class QuizQuestionTableHelper extends QuestionTableHelper
{
	function QuizQuestionTableHelper()
	{
		
	}
	
	/**
	 * 
	 * @param $obj - instance of a survey question xref class or question class
	 */
	function formatOptions($obj)
	{
		if(!$obj->hasField("question_type_id") && $obj->hasRelation(Question))
		{
			$question = $obj->Question();
			if (!$question) return "";
		}
		else
		{
			$question = $obj;
		}
		
		if($question->isFreeText() || $question->isShortText())
		{
			$text .= "Character Limit: ";
			$text .=  ($question->char_limit) ? $question->char_limit : "None";
			$text .= "<br>";
		}
		else
		{
			$text =  preg_replace("/\s+\n/", "; ", $question->options);
		}

		if($question->isFreeText())
			$text .= "Number of Rows: " . $question->num_rows . "<br>";
			
		return $text;
	}
	
	
	static function formatCorrectAnswer($question)
	{
		if(!$question->hasField("answer") || !$question->answer || !$question->options)
		{
			return "";
		}
		
		$options = explode("\n", $question->options);
		return $options[$question->answer-1];
	}	
}




class QuestionFormSetup
{
	function QuestionFormSetup()
	{

	}

	/**
	 * When loading a question, setup the form based on the question type.
	 * 
	 * @param AutoForm obj $form - 
	 * @param obj $question - obj of class Question or custom Question DataItem;
	 * obj must have the following functions:
	 * function isFreeText()
	 * function isShortText()
	 * function isCheckList()
	 * function isRatings()
	 * 
	 * and a field "options" that contains the set options for
	 * select, checklist, multichoice and ratings questions
	 * 
	 * @param boolean $editable - whether the question can be edited
	 */
	static function questionTypeSetup(&$form, $question, $editable)
	{
		$pk = $question->getPrimaryKey();
		
		if($question->isRatings() || $editable)
		{
			$form->add(new RatingQuestionFieldRenderer($form), "label_for_lowest");
			$form->add(new StringFieldRenderer($form), "label_for_highest");
			$form->add(new NumberFieldRenderer($form), "number_of_steps");	
		}
		
		if($editable || $question->isFreeText() || $question->isShortText())
		{
			$form->getRenderer("num_rows")->size = 5;
			$form->getRenderer("char_limit")->size = 5;
		}
		else
			$form->hide("num_rows", "char_limit");

		if($question->$pk && $question->isRatings())
			$form =  QuestionFormSetup::setOptionsToRatings($form);
		elseif($editable)
			$form = QuestionFormSetup::setDefaultRatings($form);

		$form->labelCSS = "question_form_label";
				
		return $form;
	}

	static function setRatingsToOptions()
	{
		global $_POST;

		$options = $_POST["label_for_lowest"] . "\r\n";
		$options .= $_POST["label_for_highest"] . "\r\n";
		$options .= $_POST["number_of_steps"] . "\r\n";

		$_POST["options"] = $options;
	}

	static function setOptionsToRatings($form)
	{
		list($from, $to, $steps) = explode("\n", $form->data->get("options"));

		$form->data->set("label_for_lowest", $from);
		$form->data->set("label_for_highest", $to);
		$form->data->set("number_of_steps", $steps);

		return $form;
	}

	static function setDefaultRatings($form)
	{
		$form->data->set("label_for_lowest", "Lowest");
		$form->data->set("label_for_highest", "Highest");
		$form->data->set("number_of_steps", 5);

		return $form;
	}
}

?>