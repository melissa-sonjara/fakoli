<?php
/**************************************************************

 Copyright (c) 2007-2010 Sonjara, Inc

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

/**
 * 
 */

require_once realpath(dirname(__FILE__))."/data_item.inc";

/**
 * Base class for Validators.
 * 
 * 
 */
class AbstractValidator
{
	var $field;
	var $title;
	
	function AbstractValidator($field, $title)
	{
		$this->field = $field;
		$this->title = $title;
	}
	
	function writeClient()
	{
	}
	
	function validate()
	{
	}
}

/**
 * RequiredField Validator.
 * 
 * 12/18/09, JDG, added checkDisplayNone to enable
 * some renderers to have required validation whether
 * displayed or not (e.g., images with LOC_key fields)
 */
class RequiredValidator extends AbstractValidator
{
	var $empty = "";
	var $checkDisplayNone = true;

	function RequiredValidator($field, $title, $empty = "", $checkDisplayNone = true)
	{
		$this->empty = $empty;
		$this->AbstractValidator($field, $title);
		$this->checkDisplayNone = $checkDisplayNone;
	}
	
	function writeClient()
	{
		if ($this->readOnly) return "";
		
		if($this->checkDisplayNone)
			$condition = "form[\"{$this->field}\"] != null && form[\"{$this->field}\"].style.display != \"none\" && ";
				
		$script = <<<ENDSCRIPT
		
	if ({$condition}form["{$this->field}"].value == "{$this->empty}")
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
		
		if (!isset($_POST[$this->field]))
		{
			return "{$this->title} is a required field.";
		}
		else
		{
			return "";
		}
	}
}


class RequiredFileValidator extends AbstractValidator
{
	var $message;
		
	function RequiredFileValidator($field, $title, $message = "")
	{
		$this->AbstractValidator($field, $title);
		$this->message = $message ? $message : "{$title} is a required field. Please browse for a file using the 'Browse' button";

		trace("RequiredValidator($field,$title,$message)", 3);
	}
	
	function writeClient()
	{
		if ($this->readOnly) return "";
		
		$msg = jsSafe($this->message, true);
		$script = <<<ENDSCRIPT
		
	if (form["{$this->field}"].style.display != "none" && form["{$this->field}"].value == "{$this->empty}")
	{
		alert("{$msg}");
		return false;
	}
ENDSCRIPT;

		return $script;
	}
	
	function validate()
	{
		
		if (!isset($_FILES[$this->field]))
		{
			return $this->message;
		}
		else
		{
			return "";
		}
	}
}

/**
 * Date Validator.
 * 
 * Dates are required by default. To make a date not required, set
 * the following in the calling script:
 * 
 * $dateRenderer = $form->getRenderer("date field");
 * $dateRenderer->required = false; 
 */
class DateValidator extends AbstractValidator
{
	var $required = true;
	
	function DateValidator($field, $title)
	{
		$this->AbstractValidator($field, $title);
		
		if (func_num_args() > 2)
		{
			$this->required = func_get_arg(2);
		}
	}
	
	function writeClient()
	{
		if ($this->readOnly) return "";
		
		$script = "";
		if ($this->required)
		{
			$script = <<<ENDSCRIPT
		
	if (form["{$this->field}"].value == "")
	{
		alert("{$this->title} is a required field. Please supply a value.");
		return false;
	}
ENDSCRIPT;
		}
		
		$script .= <<<ENDSCRIPT
			
	if (form["{$this->field}"].value != "" &&
		!form["{$this->field}"].value.match(/^\d{1,2}\/\d{1,2}\/\d{4}$/))
	{
		alert("{$this->title} must be in the format MM/DD/YYYY");
		return false;
	}
ENDSCRIPT;

		return $script;
	}
	
	function validate()
	{
		global $_POST;

		$result = "";
			
		if(!$this->required)
			$result = "";
		else if (!isset($_POST[$this->field]) || $_POST[$this->field] == "")
		{
			$result = "{$this->title} is a required field.";
		}
		else if (!preg_match("|^\\d{1,2}\/\\d{1,2}\/\\d{4}$|", $_POST[$this->field]))
		{
			$result = "{$this->title} must be in the format MM/DD/YYYY";
		}	
		
		return $result;
	}
}

/**
 * Uniqueness Validator. Tests to ensure that the database does not contain a record
 * which matches the supplied value for the given field (useful for usernames, etc.)
 * Obviously, this validator has no client-side component.
 * 
 * 
 */
class UniqueValidator extends AbstractValidator
{
	var $item;
	var $message;
	
	function UniqueValidator($field, $title, $item, $message)
	{
		$this->class = $item->table;
		$this->message = $message;
		$this->item =& $item;
		
		$this->AbstractValidator($field, $title);
	}

	function writeClient()
	{
		// No client-side component
		
		return "";
	}
	
	function validate()
	{
		global $_POST;
		
		$value = $_POST[$this->field];
		
		$result = "";
		
		if ($value)
		{			
			trace("UniqueValidator::validate: ".get_class($this->item), 3);
			
			$value = mysql_escape_string($value);
			
			$pk = $this->item->primary_key;
			
			$constraint = "WHERE {$this->field}='$value'";
			
			if ($this->item->$pk)
			{
				$constraint .= " AND {$pk}!='{$this->item->$pk}'";
			}
			
			$matches = $this->item->queryValue("COUNT(1)", $constraint);
			if ($matches > 0)
			{
				trace("UniqueValidator::validate - found $matches match(es), raising error", 3);
				
				$result = $this->message;
			}
		}
		
		return $result;
	}
}

/**
 * Tests whether two fields contain the same value. Used for password + confirmation forms, etc.
 */
class MatchValidator extends AbstractValidator
{
	var $confirm;
	var $confirmTitle;

	function MatchValidator($field, $title, $confirm, $confirmTitle)
	{
		$this->confirm = $confirm;
		$this->confirmTitle = $confirmTitle;
		
		$this->AbstractValidator($field, $title);
	}
	
	function writeClient()
	{
		trace("MatchValidator::writeClient", 3);
		if ($this->readOnly) return "";
		
		$script = <<<ENDSCRIPT
			
	if (form["{$this->field}"].value != form["{$this->confirm}"].value)
	{
		alert("{$this->title} and {$this->confirmTitle} do not match.");
		return false;
	}
ENDSCRIPT;

		return $script;
	}
	
	function validate()
	{
		global $_POST;
		$result = "";
		
		$value1 = $_POST[$this->field];
		$value2 = $_POST[$this->confirm];
		
		if ($value1 != $value2)
		{
			$result = "{$this->title} and {$this->confirmTitle} do not match";
		}
		
		return $result;
	}
}

/*
 * PasswordValidator
 * 
 * The password validator needs to work differently from the required
 * validator because the user does not have to enter a password unless
 * they open the reset block or if the block is opened for them for 
 * new account creation. This validator checks if the password block
 * style is open by checking if style is not "none". If style is not
 * none, then the password field cannot be empty. 
 */
class PasswordValidator extends RequiredValidator
{
	var $empty = "";
	
	function PasswordValidator($field, $title, $empty = "")
	{
		$this->empty = $empty;
		$this->AbstractValidator($field, $title);
	}

	function writeClient()
	{
		if ($this->readOnly) return "";
		
		$pswField = "{$this->field}_block";
	$script = <<<ENDSCRIPT
	
	var block = document.getElementById("$pswField");
	
	if (block.style.display != "none" && form["{$this->field}"].value == "{$this->empty}")
	{
		alert("{$this->title} is a required field. Please supply a value.");
		return false;
	}
		
	
ENDSCRIPT;

		return $script;
	}
}

/**
 * Tests wheteher a field's value matches the supplied regular expression.
 */
class RegularExpressionValidator extends AbstractValidator
{
	var $regexp;
	var $message;
	
	function RegularExpressionValidator($field, $title, $regexp, $message)
	{
		$this->regexp = $regexp;
		$this->message = $message;
		$this->item =& $item;
		
		$this->AbstractValidator($field, $title);	
	}
	
	function writeClient()
	{
		trace("RegularExpressionValidator::writeClient", 3);

		if ($this->readOnly) return "";
		
		$script = <<<ENDSCRIPT
			
	if (!form["{$this->field}"].value.match(/{$this->regexp}/i))
	{
		alert("{$this->message}");
		return false;
	}
ENDSCRIPT;
		
		return $script;
	}
	
	function validate()
	{
		//$exp = preg_replace("|([^/])|", "$1\\/", $this->regexp);
		$exp = $this->regexp;
		if (!preg_match("/{$exp}/i", $_POST[$this->field]))
		{
			$result = $this->message;
		}	
		
		return $result;
	}
}

/**
 * RangeValidator class. Checks that the user-supplied value for a given form
 * field is within the supplied range.
 */
class RangeValidator extends AbstractValidator
{
	var $min;
	var $max;
	var $message;
	
	function RangeValidator($field, $title, $min, $max, $message = null)
	{
		$this->min = $min;
		$this->max = $max;
		$this->message = $message;
		
		if (!$message)
		{
			$this->message = "$title must be in the range $min to $max";
		}
		
		$this->AbstractValidator($field, $title);	
	}
	
	function writeClient()
	{
		trace("RegularExpressionValidator::writeClient", 3);
		if ($this->readOnly) return "";
		
		$min = is_numeric($this->min) ? $this->min : "'{$this->min}'";
		$max = is_numeric($this->max) ? $this->max : "'{$this->max}'";
		
		$script .= <<<ENDSCRIPT
	
	if (form["{$this->field}"].value == "" || form["{$this->field}"].value < $min || form["{$this->field}"].value > $max)
	{
		alert("{$this->message}");
		return false;
	}
		
ENDSCRIPT;
		return $script;
	}
	
	function validate()
	{
		$val = $_POST[$this->field];
		if ($val === "" || $val < $this->min || $val > $this->max)
		{
			$result = $this->message;
		}
		
		return $result;
	}
}


/*
 * SelectFieldRequiredValidator
 * 
 * A custom required validator is needed so that When a calling scripts turns 
 * add entry text on for the instance of the SelectFieldRenderer class, 
 * we  will check if the user has either selected an option from the list or 
 * entered text because the standard required validator will consider the field 
 * not completed if the user adds entry text.
 * 
 * Currently, for this to work, the script must NOT set the field as
 * required, because that would cause the ValidationEngine to include
 * the generic RequiredValidator, which would not see text in the add
 * entry field as meeting the required condition. The "*" can be added
 * to the field label using the alias function. Add this validator the
 * the script using:
 * $form->validator->add(new SelectFieldRequiredValidator($field, $label, $empty));
 * 
 */
class SelectFieldRequiredValidator extends AbstractValidator
{
	var $empty = "";
	
	function SelectFieldRequiredValidator($field, $title, $empty="")
	{
		$this->empty = $empty;
		$this->AbstractValidator($field, $title);
	}
	
	function writeClient()
	{
		if ($this->readOnly) return "";
		
		$script = <<<ENDSCRIPT
		
		if (form["{$this->field}"].style.display != "none" && form["{$this->field}"].value == "{$this->empty}"
		&& form["{$this->field}_addEntry"].value == "{$this->empty}")
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
		
		if (!isset($_POST[$this->field]) && !isset($_POST["{$this->field}_addEntry"]))
		{
			return "{$this->title} is a required field.";
		}
		else
		{
			return "";
		}
	}
	
}


/**
 * RequiredStringValidator.
 * 
 * For search fields, a minimum string may be required.
 * Does not validate maximum - maximum would be enforced 
 * during data entry using limit, not with validation.
 * Add this validator the the script using:
 * $form->validator->add(new RequiredStringValidator($field, $label, $min));
 * * 
 */
class RequiredStringValidator extends AbstractValidator
{
	var $min = 0;

	function RequiredStringValidator($field, $title, $min = 0)
	{
		$this->min = $min;
		$this->AbstractValidator($field, $title);
	}
	
	function writeClient()
	{
		$script = <<<ENDSCRIPT
		
			var ctrl = form['{$this->field}'];
		
		if (ctrl.value.length < {$this->min})
		{
			alert("Please enter at least $this->min characters for {$this->title}.");
			return false;
		}
ENDSCRIPT;

		return $script;
	}
	
	function validate()
	{
		global $_POST;
		
		if (!isset($_POST[$this->field]))
		{
			return "Please enter at least {$this->min} for {$this->title}.";
		}
		else
		{
			return "";
		}
	}
}



/**
 * RequiredTextValidator.
 * 
 * For fields rendered with TextFieldRenderer
 *  
 * Add this validator the the script using:
 * $form->validator->add(new RequiredTextValidator($field, $label, $empty));
 */
class RequiredTextValidator extends RequiredValidator
{
	var $empty = "";
	
	function RequiredTextValidator($field, $title, $empty = "")
	{
		$this->empty = $empty;
		$this->AbstractValidator($field, $title);
	}
	
	function writeClient()
	{
		$script = <<<ENDSCRIPT
		
		
	if (form["{$this->field}"].value == "{$this->empty}")
	{
		alert("{$this->title} is a required field. Please supply a value.");
		return false;
	}
ENDSCRIPT;

		return $script;
	}
}



/**
 * RequiredCheckList Validator.
 * 
 * Verifies min count of checkboxes checked
 * 
 * Add this validator the the script using:
 * $form->validator->add(new RequiredCheckListValidator($field, $label, $min));
 */
class RequiredCheckListValidator extends AbstractValidator
{
	var $min = "";
	var $empty = "";
	var $addEntry = false;
	var $msgText = "checkbox";
	
	function RequiredCheckListValidator($field, $title, $min = 1, $addEntry = true)
	{
		$this->min = $min;
		$this->AbstractValidator($field, $title);
		$this->addEntry = $addEntry;
		if($min > 1)
			$this->msgText = pluralize($msgText);
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

	var addEntry = form["{$this->field}_addEntry"];
	
	if(addEntry && addEntry.value != "")
	{
		countChecked += 1;
	}
	
	if ({$this->min} > 0 && countChecked < {$this->min})
	{
		alert("You must check at least {$this->min} {$this->msgText} for {$this->title}.");
		return false;
	}
		
	
ENDSCRIPT;

		return $script;
	}
	
	
	function validate()
	{
		global $_POST;

		$values = $_POST[$this->field];
		
		// convert array of checks to comma delimated string and 
		// back again to eliminate blanks
		if (is_array($values))
		{
			$valueAnswer = implode(",",  array_values($values));
			$count = count(explode(",", $valueAnswer));
		}
		elseif($values)
			$count = 1;
		else
			$count = 0;
		
		if ($this->addEntry AND isset($_POST["{$this->field}_addEntry"]))
			$count += 1;
			
		if ($count < $this->min)
		{
			return "You must check at least {$this->min} {$this->msgText} for {$this->title}.";
		}
		else
		{
			return "";
		}
	}
}





/**
 * The ValidationEngine takes an array of validator objects in its constructor,
 * and is then able to generate both client- and server-side validation for HTML forms.
 * 
 * 
 */
class ValidationEngine
{
	var $validators = array();
	var $msg = "";
	var $id = "";
	
	/**
	 * Create a new ValidationEngine.
	 */
	function ValidationEngine()
	{
		$this->validators = func_get_args();
	}
	
	/**
	 * Output the client-side form validation function, based on all the validators that have been added.
	 */
	function writeScript()
	{
		$suffix = ($this->id) ? "_{$this->id}" : "";
		
		$script = <<<ENDSCRIPT

function validate{$suffix}(form)
{
ENDSCRIPT;

		foreach($this->validators as $v)
		{
			$script .= $v->writeClient();
		}
		
		$script .= <<<ENDSCRIPT

	return true;
}
		
ENDSCRIPT;

		return $script;
	}
	
	/**
	 * Add a validator.
	 */
	function add()
	{
		foreach(func_get_args() as $v)
		{
			$this->validators[] = $v;
		}
	}
	
	/**
	 * Perform server-side validation of the posted form values. The msg field of the ValidationEngine
	 * accumulates all the error messages raised by the individual validators.
	 * @return true if the form data is valid for all validators, false if one or more fields contain invalid data.
	 */
	function validate()
	{
		$result = "";
		
		foreach($this->validators as $v)
		{
			$msg = $v->validate();
			if ($msg)
			{
				$result .= $msg."<br>";
			}
		}
		
		$this->msg = $msg;
		
		return $result;
	}
	
	/**
	 * Checks to see whether the specified field has a RequiredValidator attached to it.
	 * @param $field the field to check
	 * @return true if the field is required, false if it is optional
	 */
	function isRequired($field)
	{
		foreach($this->validators as $v)
		{
			if ($v->field == $field && get_class($v) == RequiredValidator) return true;
		}
		
		return false;
	}
	
	/**
	 * Checks to see whether any fields have been marked as required. 
	 * @return true if there is at least one required field, false if no fields are required.
	 */
	function hasRequiredFields()
	{
		foreach($this->validators as $v)
		{
			if (get_class($v) == RequiredValidator) return true;
		}
		
		return false;
	}
}
?>
