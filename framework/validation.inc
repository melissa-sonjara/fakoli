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
		
		if ($this->readOnly) return "";
		
		if (!isset($_POST[$this->field]) || $_POST[$this->field] === $this->empty)
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
 * Validates a field rendered with TreeSelectFieldRenderer.
 * When a tree has more than one level that can be selected,
 * we need to determine that it is valid when at least
 * one item at any level has been selected. 
 */
class RequiredTreeValidator extends AbstractValidator
{
	var $empty = "";

	function RequiredTreeValidator($field, $title, $empty = "")
	{
		$this->empty = $empty;
		$this->AbstractValidator($field, $title);
	}
	
	function writeClient()
	{
		if ($this->readOnly) return "";
		
		$script = <<<ENDSCRIPT

		var div = document.id("{$this->field}_table");
		var valid = false;

		div.getElements("input").each(function(box)
		{
			if (box.checked == true)
				valid = true;
		});
		
		if(valid == false)
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


/*
 * When we must not set any of the buttons by default
 * and the field is required, we need this validator
 * instead of "RequiredValidator"
 * 
 * JDG 1/14/2011
 */
class RequiredRadioButtonValidator extends RequiredValidator
{
	var $empty = "";

	function RequiredRadioButtonValidator($field, $title, $empty = "")
	{
		$this->empty = $empty;
		$this->AbstractValidator($field, $title);
	}
	
	function writeClient()
	{
		if ($this->readOnly) return "";
		
		$script = <<<ENDSCRIPT

		var valid = false;

		var radios = form["{$this->field}"];
		
 		for(i=0; i < radios.length; i++)
 		{
 	    	if (radios[i].checked == true) 
	    	{
			   valid = true;
	    	}
		} 		
		 		
		if(valid == false)
		{
			alert("{$this->title} is a required field. Please supply a value.");
				return false;
		}
ENDSCRIPT;

		return $script;
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
		
		if ($this->readOnly) return "";
		
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

class RequiredIfCheckedValidator extends AbstractValidator
{
	var $message;
	var $checkbox;
	
	function RequiredIfCheckedValidator($field, $checkbox, $title, $checkboxTitle, $message = "")
	{
		$this->AbstractValidator($field, $title);
		$this->checkbox = $checkbox;
		
		$this->message = $message ? $message : "{$title} is a required field. Please supply a value or uncheck {$checkboxTitle}.";

		trace("RequiredIfCheckedValidator($field,$title,$checkboxTitle, $message)", 4);
	}
	
	function writeClient()
	{
		if ($this->readOnly) return "";
		
		$msg = jsSafe($this->message, true);
		$script = <<<ENDSCRIPT
		
	if (form["{$this->field}"].style.display != "none" && form["{$this->checkbox}"].checked && form["{$this->field}"].value == "{$this->empty}")
	{
		alert("{$msg}");
		return false;
	}
ENDSCRIPT;

		return $script;
	}

	function validate()
	{
		
		if ($this->readOnly) return "";
		
		if (!isset($_POST[$this->field]) && isset($_POST[$this->checkbox]))
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
 * Does not throw error if date field is empty - that is done by RequiredValidator.
 * 
 * Error if the date does not match the format mm/dd/yyyy or if either the
 * month, day, or year evaluates to an integer value of 0 (e.g., user enters
 * 00/00/0000).
 */
class DateValidator extends AbstractValidator
{
	function DateValidator($field, $title)
	{
		$this->AbstractValidator($field, $title);		
	}
	
	function writeClient()
	{
		if ($this->readOnly) return "";
		
		$script .= <<<ENDSCRIPT
	 	var d = form["{$this->field}"].value.split(/\D+/);

	 if (form["{$this->field}"].value != "" &&
		(!form["{$this->field}"].value.match(/^\d{1,2}\/\d{1,2}\/\d{4}$/) ||
		(form["{$this->field}"].value.match(/^\d{1,2}\/\d{1,2}\/\d{4}$/) && 
		(parseInt(d[0], 10) == 0 || parseInt(d[1], 10) == 0 || parseInt(d[2], 10) == 0))))
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
		
		if ($this->readOnly) return "";
		
		list($dd,$mm,$yyyy) = explode('/', $_POST[$this->field]);
		
		if ($_POST[$this->field] != "" AND !preg_match("|^\\d{1,2}\/\\d{1,2}\/\\d{4}$|", $_POST[$this->field]) ||
		(preg_match("|^\\d{1,2}\/\\d{1,2}\/\\d{4}$|", $_POST[$this->field]) && 
		(intval($dd) == 0 || intval($mm) == 0 || intval($yyyy) == 0)))
		{
			$result = "{$this->title} must be in the format MM/DD/YYYY";
		}	
		
		return $result;
	}
}


/**
 * @param string $field
 * @param string $title
 * @param string or Date $min
 * @param string or Date $max
 * @param string $message
 *
 * Min can be either a static date min that the field's value
 * must be set to or the name of a field whose value will be
 * compared to the given field. Date strings can be in either
 * format '2000-01-01' or '01/01/2000'
 * 
 * e.g.
 * static date param: today() or '2000-01-01'
 * field date: "end_date" where end_date is the tag for
 * a date input field displayed on the form
 */
class DateRangeValidator extends AbstractValidator
{
	function DateRangeValidator($field, $title, $min, $max, $message = null)
	{
		$this->min = $min;
		$this->max = $max;
		
		$fields = array();
		if($this->min && preg_match("|^(\\d{4})-(\\d\\d)-(\\d\\d)$|", $this->min, $fields))
		{
			$this->min = $fields[2]."/".$fields[3]."/".$fields[1];
		}

		if($this->max && preg_match("|^(\\d{4})-(\\d\\d)-(\\d\\d)$|", $this->max, $fields))
		{
			$this->max = $fields[2]."/".$fields[3]."/".$fields[1];
		}
		
		$this->message = $message;

		trace("DateRangeValidator field $field min {$this->min} max {$this->max}", 3);
		if (!$message)
		{
			$labelMin = ($this->min && preg_match("|^\\d{1,2}\/\\d{1,2}\/\\d{4}$|", $this->min)) ?
				$this->min : prettify($this->min);
			
			$labelMax = ($this->max && preg_match("|^\\d{1,2}\/\\d{1,2}\/\\d{4}$|", $this->max)) ?
				$this->max : prettify($this->max);
			
			if($this->min && $this->max)
			{
				$this->message = "$title must be in the range {$labelMin} to {$labelMax}.";
			}
			else if($this->min)
			{
				$this->message = "$title must be greater than or equal to the {$labelMin}.";
			}
			else if($this->max)
			{
				$this->message = "$title must be less than or equal to the {$labelMax}.";
			}
		}

		$this->AbstractValidator($field, $title);
	}
	
	/**
	 * If min or max set and it is a date string then validate
	 * as a date, if set and it is not a date string, then get the
	 * value from the form's field.
	 */
	function writeClient()
	{
		if ($this->readOnly) return "";

		$min = "'{$this->min}'";
		$max = "'{$this->max}'";
		
		$script .= <<<ENDSCRIPT

		var min = $min;
		var max = $max;
		var minDate = ""; 
		var maxDate = "";
		var fieldDate = "";
		
			
		if(form["{$this->field}"].value != "" && form["{$this->field}"].value.match(/^\d{1,2}\/\d{1,2}\/\d{4}$/))
		{
			form["{$this->field}"].value = Date.parse(form["{$this->field}"].value).format('%x');
			fieldDate = new Date(form["{$this->field}"].value);
		}
		
		if(max && max.match(/^\d{1,2}\/\d{1,2}\/\d{4}$/))
		{
			maxDate = new Date(max);
		}
		else if(max && form["{$this->max}"].value.match(/^\d{1,2}\/\d{1,2}\/\d{4}$/))
		{
			form["{$this->max}"].value = Date.parse(form["{$this->max}"].value).format('%x');
			maxDate = new Date(form["{$this->max}"].value);
		}

		if(min && min.match(/^\d{1,2}\/\d{1,2}\/\d{4}$/))
		{
			minDate = new Date(min);
		}
		else if(min && form["{$this->min}"].value.match(/^\d{1,2}\/\d{1,2}\/\d{4}$/))
		{
			form["{$this->min}"].value = Date.parse(form["{$this->min}"].value).format('%x');
			minDate = new Date(form["{$this->min}"].value);
		}
		
		if(fieldDate && 
			(minDate && fieldDate.valueOf() < minDate.valueOf()) ||
		 	(maxDate && fieldDate.valueOf() > maxDate.valueOf()) )
		{
			alert("{$this->message}");
			return false;
		}
	
ENDSCRIPT;

		return $script;
	}

	function validate()
	{
		global $_POST;

		if ($this->readOnly) return "";

		$min = $this->min;
		$max = $this->max;
		$minDate = "";
		$maxDate = "";
		
		if($min && !preg_match("|^\\d{1,2}\/\\d{1,2}\/\\d{4}$|", $min))
		{
			$min = $_POST[$this->min];
		}
		
		if(!preg_match("|^\\d{1,2}\/\\d{1,2}\/\\d{4}$|", $max))
		{
			$max = $_POST[$this->max];
		}

		if($min && preg_match("|^\\d{1,2}\/\\d{1,2}\/\\d{4}$|", $min))
		{
			$minDate = new DateTime($min."00:00:00");
		}
		
		if($max && preg_match("|^\\d{1,2}\/\\d{1,2}\/\\d{4}$|", $max))
		{
			$maxDate = new DateTime($max."00:00:00");
		}
		
		if($_POST[$this->field] != "" AND preg_match("|^\\d{1,2}\/\\d{1,2}\/\\d{4}$|", $_POST[$this->field]))
		{
			$fieldDate = new DateTime($_POST[$this->field]);
		}
		
		if ($_POST[$this->field] != "" AND (!preg_match("|^\\d{1,2}\/\\d{1,2}\/\\d{4}$|", $_POST[$this->field]) ||
		(($minDate && $fieldDate && $fieldDate < $minDate) || ($maxDate && $fieldDate && $fieldDate > $maxDate))))
		{
			$result = $this->message;
		}
		
		return $result;
	}
}


/**
 * TimeValidator.
 * 
 * If the user enters a value in a time field, check
 * if it is valid. 
 * 
 * The hour html tag field name is the field name with "_hh" after it.
 * The minute html tag field name is the field name with "_mm" after it.
 */
class TimeValidator extends AbstractValidator
{
	var $message;
	
	function TimeValidator($field, $title)
	{
		$this->AbstractValidator($field, $title);	
	}
		
	function writeClient()
	{
		if ($this->readOnly) return "";
	
$script = <<<ENDSCRIPT
	var hourObj = form["{$this->field}_hh"];
	var minObj = form["{$this->field}_mm"];
	
		if (hourObj.value != "" && (hourObj.value < 1 || hourObj.value > 12))
		{
			alert("{$this->title} Hours must be in the range 1 to 12");
			return false;
		}	
		
		if (minObj.value != "" && (minObj.value < 0 || minObj.value > 59))
		{
			alert("{$this->title} Minutes must be in the range 0 to 59");
			return false;
		}
		
		
ENDSCRIPT;

		return $script;
	}
}


/**
 * RequiredTimeValidator.
 * 
 * Custom validation for time fields which render the field
 * name html as x_hh and x_mm and thus cannot use the standard
 * RequiredValidator.
 */
class RequiredTimeValidator extends AbstractValidator
{
	var $empty = "";

	function RequiredTimeValidator($field, $title, $empty = "")
	{
		$this->empty = $empty;
		$this->AbstractValidator($field, $title);
	}
	
	function writeClient()
	{
		if ($this->readOnly) return "";
		
		$script = <<<ENDSCRIPT
	if ((
	form["{$this->field}_hh"] != "" && form["{$this->field}_hh"].style.display != "none" &&
	form["{$this->field}_hh"].value == "{$this->empty}") || 
	(form["{$this->field}_mm"] != "" && form["{$this->field}_mm"].style.display != "none" &&
	form["{$this->field}_mm"].value == "{$this->empty}")
	)
	{
		alert("{$this->title} is a required field. Please supply a value.");
		return false;
	}
	
	
ENDSCRIPT;

		return $script;
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
	
	function UniqueValidator($field, $title, $item, $message, $extraConstraint = "")
	{
		$this->class = $item->table;
		
		if(!$message)
		{
			$className = $item->prettifyClassName();
			$message = (preg_match("/^[aeiou]/i", $className)) ? "An " : "A ";
			$fieldName = strtolower($title);
			$message .= "{$className} already exists with that {$fieldName}. Please choose a different {$fieldName}.";
		}
		$this->message = $message;
		$this->item =& $item;
		$this->extraConstraint = $extraConstraint;
			
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
			
			$value = mysqli_escape_string($value);
			
			$pk = $this->item->primary_key;
			
			$constraint = "WHERE {$this->field}='$value'";
			
			if ($this->extraConstraint) $constraint .= " AND {$this->extraConstraint}";
			
			if ($this->item->$pk)
			{
				$constraint .= " AND {$pk}!='{$this->item->$pk}'";
			}
			
			$matches = $this->item->queryValue("COUNT(1)", $constraint);
			trace("***************count matches ". count($matches), 3);
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
	var $caseSensitive = true;
	
	function MatchValidator($field, $title, $confirm, $confirmTitle, $caseSensitive = true)
	{
		$this->confirm = $confirm;
		$this->confirmTitle = $confirmTitle;
		$this->caseSensitive = $caseSensitive;
		
		$this->AbstractValidator($field, $title);
	}
	
	function writeClient()
	{
		trace("MatchValidator::writeClient", 3);
		if ($this->readOnly) return "";
		
		if ($this->caseSensitive)
		{

			$script = <<<ENDSCRIPT
			
	if (form["{$this->field}"].value != form["{$this->confirm}"].value)
	{
		alert("{$this->title} and {$this->confirmTitle} do not match.");
		return false;
	}
ENDSCRIPT;
		}
		else
		{
			$script = <<<ENDSCRIPT
			
	if (form["{$this->field}"].value.toLowerCase() != form["{$this->confirm}"].value.toLowerCase())
	{
		alert("{$this->title} and {$this->confirmTitle} do not match.");
		return false;
	}
ENDSCRIPT;
		}
		return $script;
	}
	
	function validate()
	{
		global $_POST;
		$result = "";
		
		$value1 = $_POST[$this->field];
		$value2 = $_POST[$this->confirm];
		
		if ($this->caseSensitive)
		{
			if ($value1 != $value2)
			{
				$result = "{$this->title} and {$this->confirmTitle} do not match";
			}
		}
		else if (strtolower($value1) != strtolower($value2))
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
	var $form_id;
	
	function PasswordValidator($field, $title, $form_id = "", $empty = "")
	{
		$this->empty = $empty;
		$this->form_id = $form_id;
		$this->AbstractValidator($field, $title);
	}

	function writeClient()
	{
		if ($this->readOnly) return "";
		
		$pswField = "{$this->form_id}_{$this->field}_block";
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
	
	function validate()
	{
		global $_POST;
		
		if ($this->readOnly) return "";
		
		$reset = $_POST["{$this->form_id}_reset_password"];
		
		if ($reset == 1 AND (!isset($_POST[$this->field]) || $_POST[$this->field] === $this->empty))
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
 * Tests wheteher a field's value matches the supplied regular expression.
 * 
 * JDG 2/2012 - allow empty - validate nonempty if required
 * with RequiredValidator.
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
			
	if(form["{$this->field}"].value != "" &&
	!form["{$this->field}"].value.match(/{$this->regexp}/i))
	{
		alert("{$this->message}");
		return false;
	}
ENDSCRIPT;
		
		return $script;
	}
	
	function validate()
	{
		if ($this->readOnly) return "";
		
		//$exp = preg_replace("|([^/])|", "$1\\/", $this->regexp);
		$exp = $this->regexp;
		if ($_POST[$this->field] != "" && !preg_match("/{$exp}/i", $_POST[$this->field]))
		{
			$result = $this->message;
		}	
		
		return $result;
	}
}

/**
 * RangeValidator class. Checks that the user-supplied value for a given form
 * field is within the supplied range.
 * 
 * $form->validator->add(new RangeValidator($field, $label, $min, $max, $message));
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
		trace("RangeValidator::writeClient", 3);
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
		if ($this->readOnly) return "";
		
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
		
		if ($this->readOnly) return "";
		
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
		
		if ($this->readOnly) return "";
		
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
 * RequiredBoolean Validator.
 * 
 * Verifies single checkbox (Boolean renderer)
 * is checked. While it would seem unnecessary to
 * have a checkbox that must be checked, it is
 * useful for "agree to the terms" fields.
 * 
 * Add this validator the the script using:
 * $form->validator->add(new RequiredBooleanValidator($field, $label, $empty));
 */
class RequiredBooleanValidator extends RequiredValidator
{
	var $msg;
	
	function RequiredBooleanValidator($field, $title, $msg = "")
	{
		$this->AbstractValidator($field, $title);
		$this->msg = ($msg) ? $msg : $title . " is a required field.";
	}
	
	function writeClient()
	{		
$script = <<<ENDSCRIPT
		
	var boolObj = form["{$this->field}"];
		
	if (boolObj.checked == false)
	{
		alert("{$this->msg}");
		return false;
	}
		
	
ENDSCRIPT;

		return $script;
	}
	
	// no need to overwrite server side validation or parent class
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

		var pattern = new RegExp("^{$this->field}\\\\[.*?\\\\]$");
		
		for(i=0; i < form.elements.length; i++)
		{
			ctrl = form.elements[i];
			if (pattern.test(ctrl.name))
			{
				if (ctrl.checked == true)
				{
					countChecked++;
				}
			}
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

		if ($this->readOnly) return "";
		
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
 * RequiredCrossReferenceSelectValidator
 * 
 * Verifies min count of checkboxes checked
 * 
 * Add this validator the the script using:
 * $form->validator->add(new RequiredCrossReferenceSelectValidator($field, $label, $min));
 */
class RequiredCrossReferenceSelectValidator extends AbstractValidator
{
	var $min = "";
	var $empty = "";
	var $addEntry = false;
	var $msgText = "checkbox";
	
	function RequiredCrossReferenceSelectValidator($field, $title, $min = 1)
	{
		$this->min = $min;
		$this->AbstractValidator($field, $title);
		$this->addEntry = $addEntry;
		if($min > 1)
			$this->msgText = pluralize($msgText);
	}
	
	function writeClient()
	{	
		if($this->min == 0) return;
			
$script = <<<ENDSCRIPT
		
		var i = 1;
		var countChecked = 0;
		var max = form["count_{$this->field}"].value;
	
		var pattern = new RegExp("^{$this->field}\\\\[.*?\\\\]$");
		
		for(i=0; i < form.elements.length; i++)
		{
			ctrl = form.elements[i];
			if (pattern.test(ctrl.name))
			{
				if (ctrl.checked == true)
				{
					countChecked++;
				}
			}
		}
		
	if (countChecked < {$this->min})
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

		if ($this->readOnly) return "";
		
		if (isset($_POST[$this->field]))
		{
			$count = count($_POST[$this->field]);
		}		
				
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
 * RequiredCurrencyField Validator.
 *
 * Currency formats as 0.00; if required, then make 
 * sure the value is not 0.00
 */
class RequiredCurrencyValidator extends RequiredValidator
{
	function RequiredCurrencyValidator($field, $title)
	{
		$this->empty = 0;
		$this->AbstractValidator($field, $title);
	}
	
	function writeClient()
	{
		if ($this->readOnly) return "";
		
		$script = <<<ENDSCRIPT
		
		var num =  parseFloat(form["{$this->field}"].value);
					
	if (num == $this->empty)
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
		
		if (!isset($_POST[$this->field]) OR floatval($_POST[$this->field]) == $this->empty)
		{
			return "{$this->title} is a required field.";
		}
		else
		{
			return "";
		}
	}
	
}

class CreditCardNumberValidator extends AbstractValidator
{
	function CreditCardNumberValidator($field, $title)
	{
		$this->AbstractValidator($field, $title);
	}
	
	function writeClient()
	{		
$script = <<<ENDSCRIPT

		if (form["{$this->field}"].value && 
			(!luhnCheck(form["{$this->field}"].value) || 
			form["{$this->field}"].value.length < 13 || 
			form["{$this->field}"].value.length > 19))
		{
			alert("{$this->title} is not valid.");
			return false;
		}
		
ENDSCRIPT;

		return $script;
	}
	
	function validate()
	{
		global $_POST;

		if ($this->readOnly) return "";
		
		$val = $_POST[$this->field];
		$len = strlen($val);
		
		if (!luhnTest($len) || $len < 13 || $len > 19)
		{
			return "{$this->title} is not valid.";
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
	var $validators = array(); 	///< The array of validators making up this validation engine
	var $msg = "";				///< Validation error messages accumulator;
	var $id = "";				///< The ID of this validator
	var $generateScript = true;	///< Specifies whether to generate client-side validation script
	
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

		if ($this->generateScript)
		{ 
			foreach($this->validators as $v)
			{
				$script .= $v->writeClient();
			}
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
	 * Get the specified validator by field and class
	 * @param unknown_type $field the validator's field
	 * @param unknown_type $class the validator's class
	 */
	function getValidator($field, $class)
	{
		foreach($this->validators as $v)
		{
			if ($v->field == $field && get_class($v) == $class)
			{
				return $v;
			}
		}
		
		return null;
	}
	
	/**
	 * Get the validators attached to a specific field
	 * @param $field the field
	 */
	function getValidators($field)
	{
		$vals = array();
		foreach($this->validators as $v)
		{
			if ($v->field == $field)
			{
				$vals[] = $v;
			}
		}
		
		return $vals;
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



/**
 * When the data entry interface contains multiple tabs (pages)
 * that the user can move around freely (e.g., not required to 
 * complete tab c before moving on to tab d) then we validate on 
 * the final submit tab. 
 * 
 * The warning message is called from the final tab's summary 
 * view display for each section and on the tab page the user 
 * is redirected to if they attempt to submit when there is an
 * error. 
 * 
 * E.g., user does not complete grant application.
 * On the view summary section
 * "Grant Application"
 * 
 * we write warning: "You must complete the grant application"
 * 
 * @author Janice Gallant for Sonjara, Inc.
 * 
 * 5/3/2011
 */
abstract class ValidateTabSet
{
	var $msg;
	
	function ValidateTabSet()
	{	
	}
	
	/**
	 * Called when user clicks submit: validate each tab page and 
	 * redirect to first page that is invalid or returns true.
	 */
	function Validate()
	{	
	}
	
	/*
	 * Additional functions as required to validate each page
	 * in the tab set. For example:
	 * 
	 * function validateGrant()
	 * 
	 * function validateProgram()
	 * 
	 * function validateOrder()
	 * 
	 * these functions set msg variable to the appropriate error/warning
	 * message.
	 */
	
	
	/**
	 * Called from summary view class for each section if
	 * the section is invalid.
	 * 
	 * The div must be an id for the background image to
	 * show - does not work for css classes.
	 */
	function warning()
	{
		if($this->msg)
		{
			echo "<div id='warning'>{$this->msg}</div>\n";
		}	
	}	
	
	/**
	 * Show the validation issue/error as an informative message 
	 * instead of a warning.
	 */
	function message()
	{
		if($this->msg)
		{
			echo "<p>{$this->msg}</p>\n";
		}	
	}	
	
	/**
	 * When system returns user to a tab b/c not
	 * valid or completed, show a popup message
	 * for the error.
	 */
	function writeScript()
	{
		$msg = stripHTML($this->msg);
		if($msg)
		{
		
$script .= <<<ENDSCRIPT
<script type="text/javascript">
window.addEvent('domready', function()
{
	alert('$msg');
});
</script>
ENDSCRIPT;
		}
		return $script;		
	}		
}



/**
 * Validates that the user's supplied password is correct.
 * @author andy
 *
 */
class SignatureValidator extends AbstractValidator
{
	var $msg;					///< The error message to report if there is a mismatch
	var $empty = "";			///< Empty value for the control
	var $test = "password";		///< Field to match
	
	/**
	 * Create a new SignatureValidator
	 * @param string $field
	 * @param string $title
	 * @param string $msg
	 */
	function SignatureValidator($field, $title, $msg, $test = "password")
	{
		$this->AbstractValidator($field, $title);
		$this->msg = $msg;
		$this->test = $test;
	}

	function writeScript()
	{
		// no client side validation
	}

	function validate()
	{
		global $_POST;
		global $user;

		$value = $_POST[$this->field];

		if (isset($value) && $value !== $this->empty)
		{
			if ($this->test == "password")
			{
				$hash = $user->hashPassword($value, $user->password);
	
				if($user->password == $hash)
					return "";
				else
				{
					trace("Signature Validator::invalid signature", 3);
					return $this->msg;
				}
			}
			else
			{
				if ($user->get($this->test) == $value) return "";
				return $this->msg;
			}
		}
	}
}

?>