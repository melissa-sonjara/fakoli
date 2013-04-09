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

 /* Title: validation.inc
 * 
 * Description: Provides validation for individual questions
 * in the questionnaire.
 * 
 * author: Janice Gallant for Sonjara, Inc.
 * 
 * date: 11/4/09
 * 
*/

Fakoli::usingFeature("validation");

/**
 * RequiredQuestion Validator.
 * 
 * 
 */
class RequiredQuestionValidator extends AbstractValidator
{
	var $empty = "";
	var $answer; // value of answer object, if view
	
	function RequiredQuestionValidator($field, $title, $empty = "", $answer = "")
	{
		$this->empty = $empty;
		$this->answer = $answer;
		$this->AbstractValidator($field, $title);
	}
	
	function writeClient()
	{
		$script = <<<ENDSCRIPT
		
	if (form["{$this->field}"].style.display != "none" && form["{$this->field}"].value == "{$this->empty}")
	{
		alert("{$this->title} is a required field. Please supply a value.");
		return true;
	}
ENDSCRIPT;

		return $script;
	}
	
	function validate()
	{
		if(!$this->answer)
		{
			global $_POST;
			$value = "";
			if($_POST AND (isset($_POST[$this->field]) || $_POST[$this->field] != ""))
			{
				$value = $_POST[$this->field];
			}
		}
		else
		{
			$value = $this->answer->value;
		}
		
		if ($value != $this->empty)
		{
			return "";
		}
		else
		{
			return "{$this->title} is a required field.";
		}
	}
}


/**
 * RequiredCheckListQuestion Validator.
 * 
 * Verifies min count of checkboxes checked
 */
class RequiredCheckListQuestionValidator extends RequiredValidator
{
	var $min;
	var $answer;
	var $text;
	
	function RequiredCheckListQuestionValidator($field, $title, $min = 0, $answer = "")
	{
		$this->min = $min;
		$this->answer = $answer;
		$this->AbstractValidator($field, $title);
		
		$this->text = ($min < 2) ? "checkbox" : "checkboxes";
	}
		
	function writeClient()
	{
		$script = <<<ENDSCRIPT
		
		var i = 1;
		var countChecked = 0;
		var max = form["count_{$this->field}"].value;
		var fieldName;

		for(i=1; i <= max; i++)
		{
			fieldName = form["{$this->field}["+i+"]"];
			if(fieldName.checked == true)
				countChecked += 1;
		}
			
	if ({$this->min} > 0 && countChecked < {$this->min})
	{
		alert("You must check at least {$this->min} {$this->text} for {$this->title}.");
		return false;
	}
ENDSCRIPT;

		return $script;
	}
	
	function validate()
	{
		if(!$this->answer)
		{
			global $_POST;
			$values = $_POST[$this->field];
			// convert array of checks to comma delimated string and 
			// back again to eliminate blanks
			if (is_array($values))
				$valueAnswer = implode(",",  array_values($values));		
		}
		else
		{
			$valueAnswer = $this->answer->value;
		}

		if (strlen($valueAnswer) > 1)
			$count = count(explode(",", $valueAnswer));
		elseif($valueAnswer)
			$count = 1;
		else
			$count = 0;
		
		if ($count < $this->min)
		{
			return "You must check at least {$this->min} {$this->text} for {$this->title}.";
		}
		else
		{
			return "";
		}
	}
}



/**
 * RequiredRadioButtonQuestion Validator.
 * 
 * Validates one radio button is checked
 */
class RequiredRadioButtonQuestionValidator extends RequiredValidator
{
	var $answer;
	
	function RequiredRadioButtonQuestionValidator($field, $title, $answer = "")
	{
		$this->answer = $answer;
		$this->AbstractValidator($field, $title);
	}
		
	function writeClient()
	{
		$script = <<<ENDSCRIPT

		var i = 1;
		var countChecked = 0;
		var max = form["count_{$this->field}"].value;

		var fieldName;
		for(i=1; i <= max; i++)
		{
			fieldName = form["{$this->field}_"+i];
			if(fieldName.checked)
				countChecked += 1;
		}
			
			
	if (countChecked < 1)
	{
		alert("You must select an option for {$this->title}.");
		return false;
	}
	
ENDSCRIPT;

		return $script;
	}
	
	function validate()
	{
		if(!$this->answer)
		{
			global $_POST;
			$value = $_POST[$this->field];
		}
		else
		{
			$value = $this->answer->value;
		}
		
		if(!$value)
		{
			return "You must select an option for {$this->title}.";
		}
		else
		{
			return "";
		}
	}
}

/**
 * If a survey allows anonymous responses, then email message is only
 * required if the user enters a recipient, additional recipient, or 
 * cc recipient.
 * 
 * This validator is only used in the case of survey
 * allow_anonymous_responses == true.
 * 
 * @author janice
 *
 */
class RequiredQuestionnaireMessageValidator extends RequiredValidator
{
	var $conditionalFields = array();
	
	function __construct($form, $field, $title = "", $message = "")
	{
		$this->AbstractValidator($field, $title);
		$this->conditionalFields[] = "recipients";
		
		if($form->data->hasField("additional_recipients"))
		{
			$this->conditionalFields[] = "additional_recipients";
		}
		
		if($form->data->hasField("cc_recipients"))
		{
			$this->conditionalFields[] = "cc_recipients";
		}	
	}
	
	function writeClient()
	{
		if ($this->readOnly) return "";
		
		if(count($this->conditionalFields) > 0)
		{
			$conditions = array();
			foreach($this->conditionalFields as $field)
			{
				$conditions[] = "form[\"{$field}\"].value != \"{$this->empty}\"";
			}
			$conditions = "(" . implode(" || ", $conditions) . ") && ";
		}
		
		$script = <<<ENDSCRIPT
		
	if ({$conditions}form["{$this->field}"].value == "{$this->empty}")
	{
		alert("{$this->title} is a required field. Please supply a value.");
		return false;
	}
ENDSCRIPT;
		
		return $script;
	}
	
	function validate()
	{
		global $_POST;
		
		if ($this->readOnly) return "";

		$empty = true;
		if(count($this->conditionalFields) > 0)
		{
			foreach($this->conditionalFields as $field)
			{
				if(isset($_POST[$this->field]) && $_POST[$this->field] !== $this->empty)
				{
					$emtpy = false;
				}
			}
		}
			
		if ((!isset($_POST[$this->field]) || $_POST[$this->field] === $this->empty) 
				&& (!count($this->conditionalFields) || !$empty))
		{
			return "{$this->title} is a required field.";
		}
		else
		{
			return "";
		}
	}
}
