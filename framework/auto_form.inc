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

require_once realpath(dirname(__FILE__)."/field_renderers.inc");
require_once realpath(dirname(__FILE__)."/validation.inc");

/**
 * AutoForm automatically creates a form based on an underlying DataItem.
 *
 */
class AutoForm
{
	var $formCSS = "";					///< CSS Class to use for the table containing the form
	var $labelCSS = "";					///< CSS Class to use for label cells
	var $valueCSS = "";					///< CSS Class to use for field/value cells
	var $buttonCSS = "";				///< CSS Class to use for buttons
	var $inputCSS = "";					///< CSS Class to use for input fields
	var $checkboxCSS = "";				///< CSS Class to use for checkbox fields
	var $style = "";
	var $componentPath ="";
	var $passwordEncryptor = "";		///< Password Encryption method name
	
	var $capitalizationMode = "word";	///< Capitalization to be used for labels. Can be 'lower', 'upper', 'first' or 'word'. 
	
	var $allowDelete = false;			///< If true, the form will display a delete button if editing an existing item.
	
	var $method = "POST";				///< HTTP method that will be used to submit the form.
	var $action = "";					///< Action URL for submitting the form. Generally this can be left blank to submit back to the same page.
	
	var $renderers = array();			///< The field renderers collection
	var $overrides = array();			///< The field overrides collection
	var $hidden = array();				///< The hidden fields collection
	var $additional = array();			///< The additional (pseudo) fields collection
	var $readonly = array();			///< The read-only fields collection
	var $annotations = array();			///< The field annotations collection
	var $buttons = array();				///< The custom buttons collection
	
	var $validator;						///< The ValidationEngine for this form
	var $useLinkSubmit = false;			///< Flag to indicate whether the submit button for the form should be rendered as a link (not common).
	
	var $HTMLEditorHideStyleBar = false;	///< Flag to indicate that Rich Text Editors should be displayed with or without the style editing toolbar.
	var $HTMLEditorDefaultWidth = "540px";	///< Default width for Rich Text Editor controls.
	var $HTMLEditorDefaultHeight = "200px";	///< Default height for Rich Text Editor controls.
	
	var $onSaveComplete = null;	///< Callback event handler that is fired after the AutoForm has finished saving data to the database.
	
	var $onDelete = "";						///< delete method name
	var $markRequiredFields = false;		///< Set to true to add an asterisk after required field labels.
	var $onFormatLabel = null; ///< Label Processing hook
	
	/**
	 * Creates a new AutoForm, based on the supplied target DataItem.
	 *
	 * @param DataItem $target the DataItem object upon which the form will be based
	 * @param string $method the HTTP method to be used for submitting the form (default is POST)
	 * @param string $action the HTTP action URL to be used for submitting the form (default is to post back to the originating URL)
	 * @param string $id DOM element ID to use for the form when rendering to HTML. Useful for pages with more than one form.
	 * @return AutoForm
	 */
	function AutoForm($target, $method="POST", $action="", $id="")
	{
		global $auto_form_defaults;
		
		$this->validator = new ValidationEngine();
		
		foreach($auto_form_defaults as $field => $value)
		{
			$this->$field = $value;
		}
		
		$this->data =& $target;
		$this->method = $method;
		$this->action = $action;
		$this->id = ($id != "") ? $id : get_class($this->data)."_form";

		$this->validator->id = $this->id;
		
		// Add in a default renderer for each field
		foreach($this->data->fields as $field => $type)
		{
			$this->renderers[$field] = $this->createRenderer($field);
		}
	}

	
	/**
	 * This factory method creates a FieldRender based on the type
	 * of the specified field.
	 *
	 * @param string $field the field for which the FieldRenderer is to be created
	 * @return FieldRenderer a FieldRenderer object suitable for the specified field.
	 */
	function createRenderer($field)
	{
		if (!array_key_exists($field, $this->data->fields)) return null;
		
		$type =$this->data->fields[$field] ;
		switch($type)
		{
		case String: 
			return new StringFieldRenderer($this);
			
		case HTML:
			return new HTMLFieldRenderer($this);
			
		case Date:
			return new DateFieldRenderer($this);
			
		case Number:
			return new NumberFieldRenderer($this);
			
		case Currency:
			return new CurrencyFieldRenderer($this);
			
		case Boolean:
			return new BooleanFieldRenderer($this);
			
		case Password:
			return new PasswordFieldRenderer($this);
			
		case Text:
			return new TextFieldRenderer($this);
			
		case Time:
			return new TimeFieldRenderer($this);
			
		case Rating:
			return new RatingFieldRenderer($this);
			
		case Timestamp:
			
			return null;
			
		default:
			die("AutoForm::createRenderer(): Unknown type");
		}
	}
	
	/**
	 * Creates default validators for fields in the form. Currently this adds Date format validators
	 * to all date fields - other fields are not affected.
	 * 
	 * This method is called automatically before the form is rendered.
	 *
	 */
	function addDefaultValidators()
	{
		
		foreach(array_keys($this->data->fields) as $field)
		{
			if ($this->data->filter && $this->data->filter->isExcluded($field)) continue;
			
			$renderer = $this->getRenderer($field);
			if ($renderer)
			{
				$renderer->addValidatorsToForm($field);
			}
		}
				
		foreach($this->additional as $r)
		{
			$r['renderer']->addValidatorsToForm($r['field']);
		}
	}
	
	/**
	 * This method writes any Javascript code that will be required for the generated
	 * form to operate, including code for Rich Text Editors, Date Pickers and field validation.
	 *
	 * @return string the script required for the generated form.
	 */
	function writeScript()
	{
		$this->patchUpReferences();
		
		ob_start();

		$this->addDefaultValidators();
		
		$obj =& $this->data;
		$script = "";				
		foreach(array_keys($obj->fields) as $field)
		{
			if ($field != $pk&& 
				!($obj->filter && $obj->filter->isExcluded($field)))
			{
				$renderer = $this->getRenderer($field);
				if ($renderer)
				{
					$renderer->renderScript($field);
				}
			}		
		}
		
		foreach($this->additional as $r)
		{
			$r['renderer']->renderScript($r['field']);
		}
				
		echo "<script type='text/javascript'>\n";
		echo $this->validator->writeScript();
		echo "\n\n";
		echo "function onSubmit{$this->id}(form)\n{\n";
		echo "\tif (!validate_{$this->id}(form)) return false;\n";
		
		foreach(array_keys($obj->fields) as $field)
		{
			if ($field != $pk&& 
				!($obj->filter && $obj->filter->isExcluded($field)))
			{
				$renderer = $this->getRenderer($field);
				if ($renderer)
				{
					$renderer->renderOnSubmitHandler($field);
				}
			}		
		}

		foreach($this->additional as $r)
		{
			$r['renderer']->renderOnSubmitHandler($r['field']);
		}
				
		echo "\treturn true;\n}\n</script>\n";	

		$script = ob_get_contents();
		ob_end_clean();
		return $script;
	}
	
	/**
	 * Retrieves the FieldRenderer object for the specified field. You can use
	 * this to access the underlying field renderer for a given field and change
	 * the way it is rendered or processed.
	 *
	 * @param string $field the name of the field for which to retrieve the FieldRenderer
	 * @return FieldRenderer
	 */
	function getRenderer($field)
	{
		if (array_key_exists($field, $this->overrides))
		{
			if ($this->overrides[$field]['renderer']) return $this->overrides[$field]['renderer'];
		}
		
		if (array_key_exists($field, $this->renderers))
		{
			return $this->renderers[$field];
		}
		
		echo("AutoForm::getRenderer(): Unknown field: $field");
		echo "<pre>";
		print_r($this->renderers);
		echo "</pre>";
	}
	
	/**
	 * Overrides the defaults for the specifed form field, providing a new
	 * label and/or FieldRenderer.
	 *
	 * @param string $field the name of the field that is to be overridden
	 * @param string $label the new text to use for the label for the specified field (null or "" to use the default label).
	 * @param FieldRenderer $renderer the FieldRenderer-derived object to use in place of 
	 * the default renderer for the specified field.
	 */
	function override($field, $label = "", $renderer = null)
	{
		if (!isset($this->overrides[$field]))
		{
			$this->overrides[$field] = array('label' => $label, 'renderer' => $renderer);
		}
		else
		{
			if ($label) $this->overrides[$field]['label'] = $label;
			if ($renderer) $this->overrides[$field]['renderer'] = $renderer;
		}
	}
	
	/**
	 * Adds an additional (pseudo) field to the form. This can be used to render
	 * additional fields in the user interface that are not directly tied to single
	 * fields in the underlying DataItem (for instance, to provide a checkbox list for
	 * many-to-one relationships, etc)
	 *
	 * @param FieldRenderer $renderer the FieldRenderer-derived object to use for the 
	 * additional field.
	 */
	function add($renderer, $field = "")
	{
		array_push($this->additional, array('renderer' => $renderer, 'field' => $field));
	}
	
	/**
	 * Call this method to hide one or more fields in the form's user interface. 
	 * For instance, calling hide("field1", "field3") would remove field1 and field3
	 * from display when the form is rendered. Note that the values for these fields
	 * are still carried around with the form and saved to the database when the
	 * AutoForm saves the target DataItem. To prevent a field on the DataItem from
	 * being processed by the AutoForm at all, you should use an InclusionFilter or 
	 * ExclusionFilter on the target DataItem. 
	 */
	function hide()
	{
		foreach(func_get_args() as $hidden)
		{
			$this->hidden[$hidden] = true;
		}
	}
	
	/**
	 * Adds an annotation (such as brief explanatory text) to the specified field.
	 * The annotation is displayed next to the field when the form is renderered.
	 *
	 * @param string $field the name of the field to annotate
	 * @param string $text the annotation text
	 */
	function annotate($field, $text)
	{
		$this->annotations[$field] = $text;
	}
	
	/**
	 * Use this method to override the display name for fields in the form.
	 * For instance, calling alias('x1', 'Useful Label') would display the text
	 * 'Useful Label' as the label for field 'x1' when rendering the form. You can
	 * alias more than one field with a single call to alias, simply by passing in
	 * more than one pairs of strings. For instance, you might calll:
	 * alias('x1', 'Field 1', 'x2', 'Field 2', ...)
	 *
	 */
	function alias()
	{
		for($i = 0; $i < func_num_args(); $i += 2)
		{
			$field = func_get_arg($i);
			$label = func_get_arg($i + 1);
			$this->override($field, $label);
		}
	}
	
	/**
	 * Call this method to make one or more fields read-only in the form's user interface.
	 * For instance, calling readonly("field1", "field3") would cause field1 and field3
	 * to be rendered read-only.
	 */
	function readonly()
	{
		foreach(func_get_args() as $readonly)
		{
			$this->readonly[$readonly] = true;
		}
	}
	
	/**
	 * Call this method to mark one or more fields as being required fields. These fields
	 * will be assigned validators to ensure that the user enters a value for each required 
	 * field. For instance, calling required("field1", "field3") would cause field1 and field3
	 * to be required fields.
	 */
	function required()
	{
		foreach(func_get_args() as $required)
		{
			if ($this->data->hasField($required))
			{
				$this->validator->add(new RequiredValidator($required, $this->prettifyFieldName($required)));
			}
		}
	}
	
	/**
	 * Determine whether the specified field is a required field for the form.
	 * @param $field the field to check
	 * @return boolean true if the field is required to be filled in by the user, false if it is an optional field.
	 */
	function isRequired($field)
	{
		return $this->validator->isRequired($field);
	}
	
	/**
	 * Validate the user input for the specified field against a regular expression, outputting
	 * the supplied message if the entered value does not match. This can be used to provide general
	 * format-based validation for fields.
	 *
	 * @param string $field the field to be validated
	 * @param string $expr the Regular Expression against which to validate
	 * @param string $message the message to display if the match fails.
	 */
	function regexp($field, $expr, $message)
	{
		if ($this->data->hasField($field))
		{
			$this->validator->add(new RegularExpressionValidator($field, $this->prettifyFieldName($field), $expr, $message));
		}
	}
	
	/**
	 * Adds a uniqueness validator to the specified field(s). This is a server-side only validator
	 * that checks that the supplied value for the field is not present for any row in the underlying
	 * database table.
	 */
	function unique()
	{
		for($i = 0; $i < func_num_args(); $i += 2)
		{
			$field = func_get_arg($i);
			$message = func_get_arg($i + 1);
			
			if ($this->data->hasField($field))
			{
				$this->validator->add(new UniqueValidator($field, $this->prettifyFieldName($field), $this->data, $message));		
			}
		}
	}
	
	/**
	 * Adds a range validator to the specified field.
	 */
	function range($field, $min, $max, $message = null)
	{
		if ($this->data->hasField($field))
		{
			$this->validator->add(new RangeValidator($field, $this->prettifyFieldName($field), $min, $max, $message));
		}
	}
	
	/**
	 * Specifies that two field values must match (as in a password/confirmation pair, for instance).
	 *
	 * @param string $field The name of the first field that must match
	 * @param string $title
	 * @param string $match The name of the second field that must match
	 * @param string $matchTitle
	 */
	function match($field, $title, $match, $matchTitle)
	{
		$this->validator->add(new MatchValidator($field, $title, $match, $matchTitle));
	}
	
	function password($field, $title)
	{
			$this->validator->add(new passwordValidator($field, $title));
	}
	
	/**
	 * Adds a custom button to the form.
	 *
	 * @param unknown_type $text
	 * @param unknown_type $url
	 * @param unknown_type $confirm
	 */
	function button($text, $url, $confirm = null)
	{		
		$this->buttons[] = array('text' => $text, 'url' => $url, 'confirm' => $confirm);
	}
	
	/**
	 * Renders the form to HTML.
	 *
	 */
	
	function drawForm()
	{
		$obj =& $this->data;
		$pk = $obj->primary_key;
		
		$submitLabel = $this->submitLabel;
		if ($submitLabel == "")
		{
			$submitLabel = ($obj->$pk != "" && $obj->$pk != 0) ? "Update ".$obj->prettifyClassName() : "Add ".$obj->prettifyClassName();
		}
		
		//echo "<!--\n";
		//print_r($this);
		//echo "\n-->\n";
		echo "<form id='{$this->id}' method='{$this->method}' action='{$this->action}' enctype='multipart/form-data' onsubmit='return onSubmit{$this->id}(this);'>\n";
		
		if ($this->id)
		{
			echo "<input type='hidden' name='#FORMID#' value='{$this->id}'/>";
		}
		echo "<input type='hidden' name='$pk' value='{$obj->$pk}'>\n";
		foreach(array_keys($this->hidden) as $hidden)
		{
			echo "<input type='hidden' name='$hidden' value='{$obj->$hidden}'/>\n";
		}
		
		echo "<table";
		if ($this->formCSS) echo " class='{$this->formCSS}'"; 
		if ($this->style) echo " style='{$this->style}'"; 
		echo ">\n";
		
		if ($this->msg)
		{
			echo "<tr>\n <td colspan='2'><span class='error'>{$this->msg}</span</td></tr>\n";
		}
		
		if ($this->markRequiredFields && $this->validator->hasRequiredFields())
		{
			echo "<tr>\n <td colspan='2'><b>* indicates required field</b></td></tr>\n";
		}
		
		foreach(array_keys($obj->fields) as $field)
		{
			if ($field != $pk && !array_key_exists($field, $this->hidden) &&
				!($obj->filter && $obj->filter->isExcluded($field)))
			{
				$renderer = $this->getRenderer($field);
				if ($renderer)
				{
					if ($this->readonly[$field])
					{
						$renderer->renderReadOnly($field);
					}
					else
					{
						$renderer->renderField($field);
					}
				}
			}
		}
		
		foreach($this->additional as $r)
		{
			$renderer = $r['renderer'];
			$field = $r['field'];
			
			if ($this->readonly[$field])
			{
				$renderer->renderReadOnly($field);
			}
			else
			{			
				$renderer->renderField($field);
			}
		}
		
		echo "<tr>\n";
		echo "<td colspan='2' class='submit'><br/>";
		if ($this->useLinkSubmit)
		{
			echo "<a class='{$this->buttonCSS}' name='submit' onclick='if (onSubmit{$this->id}(document.forms.{$this->id})) return document.forms.{$this->id}.submit(); else return false;'>$submitLabel</a>";
		}
		else
		{
			echo "<input type='submit' class='{$this->buttonCSS}' name='submit' value='$submitLabel'/>";
		}
		
		if ($obj->$pk && $this->allowDelete)
		{
			echo "&nbsp;&nbsp;&nbsp;&nbsp;<input type='submit' class='{$this->buttonCSS}' name='{$this->id}_delete' 
			      value='Delete this ".$obj->prettifyClassName()."'
			      onclick='return confirm(\"Are you sure you want to delete this ".$obj->prettifyClassName()."?\");'/>";
		}
		
		$this->drawButtons();
				
		echo "</td>";
		echo "</table>\n</form>\n";
	}
	
	/**
	 * Draws any additional buttons specified in the calling script.
	 *
	 */
	function drawButtons()
	{
		foreach($this->buttons as $button)
		{
			if ($button['confirm'])
			{
				$link = "if (confirm('".jsSafe($button['confirm'])."')) go('{$button['url']}'); return false;";
			}
			else
			{
				$link = "go('{$button['url']}'); return false;";
			}
			
			echo "&nbsp;&nbsp;&nbsp;&nbsp;<input type='button' class='{$this->buttonCSS}' onclick=\"$link\" value=\"{$button['text']}\"/>";
		}
		
	}
	
	/**
	 * Draws the form in read-only mode. This is most often used to display
	 * the details for a DataItem on a preview or details screen.
	 */
	function drawReadOnly()
	{
		echo "<table";
		if ($this->formCSS) echo " class='{$this->formCSS}'"; 
		if ($this->style) echo " style='{$this->style}'"; 
		echo ">\n";
		
		$obj =& $this->data;
		$pk = $obj->primary_key;
		
		foreach(array_keys($obj->fields) as $field)
		{
			if ($field != $pk && $obj->$field != "" && !array_key_exists($field, $this->hidden) &&
				!($obj->filter && $obj->filter->isExcluded($field)))
			{
				$renderer = $this->getRenderer($field);
				if ($renderer)
				{
					$renderer->renderReadOnly($field);
				}
			}
		}
			
		foreach($this->additional as $r)
		{
			$renderer = $r['renderer'];
			$field = $r['field'];
			$renderer->renderReadOnly($field);
		}
		
		echo "</table>";
	}
	
	/**
	 * Generates a human-readable label from the given field name. If an explicit alias has
	 * been specified using the alias() method, then that is returned. If not, a 
	 * simple set of text replacements is performed.
	 *
	 * @param string $field the field to prettify
	 * @return string
	 */
	function prettifyFieldName($field)
	{
		if (array_key_exists($field, $this->overrides))
		{
			$l = $this->overrides[$field]['label'];
			if ($l != "") return $l;
		}
		
		$field = preg_replace("/([a-z])([A-Z0-9])/", "$1 $2", $field);
		$field = str_replace("_", " ", $field);
		
		switch($this->capitalizationMode)
		{
		case "lower":

			$field = strtolower($field);
			break;
				
		case "upper":

			$field = strtoupper($field);
			break;
			
		case "first":
			
			$field = ucfirst($field);
			break;
			
		case "word":
		default:
			
			$field = ucwords($field);
			break;
		}
		
		return $field;
	}
	
	/**
	 * Loads the underlying DataItem from the database, using the specified
	 * primary key value.
	 *
	 * @param integer $id the value to use for the primary key 
	 */
	function load($id)
	{
		$this->data->load($id);	
	}
	
	/**
	 * Saves the posted values to the database.
	 *
	 * @return boolean - true if the save was successful.
	 */
	function save()
	{
		$this->patchUpReferences();
		
		$obj =& $this->data;
		
		$obj->fromPOST();
		
		if (isset($_POST["#FORMID#"]))
		{
			$this->id = $_POST["#FORMID#"];
		}
		
		if ($_POST["{$this->id}_delete"])
		{
			if(!$this->onDelete)
				$obj->delete(); // no callback function defined
			else
			{
				$deleteHandler = $this->onDelete;
				$deleteHandler($obj); // callback function
			}
			$this->deleted = true;
			return true;
		}
		
		$this->msg = $this->validator->validate();
		if ($this->msg != "") return false;
		
		foreach (array_keys($obj->fields) as $field)
		{
			if ($field != $pk  && !array_key_exists($field, $this->hidden) && 
				!($obj->filter && $obj->filter->isExcluded($field)))
			{
				$renderer = $this->getRenderer($field);
				if ($renderer)
				{
					$renderer->preProcess($field);
				}
			}		
		}		
		
		foreach($this->additional as $r)
		{
			$renderer = $r['renderer'];
			$field = $r['field'];
			
			$renderer->preProcess($field);
		}
		

		$obj->save();
		
		foreach (array_keys($obj->fields) as $field)
		{
			if ($field != $pk  && !array_key_exists($field, $this->hidden) && 
				!($obj->filter && $obj->filter->isExcluded($field)))
			{
				$renderer = $this->getRenderer($field);
				if ($renderer)
				{
					$renderer->postProcess($field);
				}
			}		
		}	

		foreach($this->additional as $r)
		{
			$renderer = $r['renderer'];
			$field = $r['field'];
			
			$renderer->postProcess($field);
		}	
		
		// onComplete event is fired once all processing has been completed
		
		if ($this->onSaveComplete) 
		{
			$onComplete = $this->onSaveComplete;			
			$onComplete($this);
		}

		return true;
	}
	
	function remove(&$obj)
	{
		$obj->delete();
	}

	/**
	 * Validate the posted values for the form. This is performed automatically
	 * when calling AutoForm::save(), but sometimes it might be necessary to 
	 * validate the form values manually (for instance, you might need to perform 
	 * some destructive database operations prior to calling AutoForm::save(), 
	 * but only if you know the form to be valid).
	 *
	 * Any validation errors encountered are stored in the form, and will be displayed
	 * when the form is subsequently rendered.
	 * 
	 * @return true if the form is valid, false otherwise.
	 */
	function validate()
	{
		$this->patchUpReferences();
		$this->msg = $this->validator->validate();
		return ($this->msg == "");
	}
	
	/**
	 * @deprecated
	 *
	 * Attempt to patch up references in the data model to cope with PHP 4's 
	 * insane object reference semantics. PHP 4 is now no longer actively 
	 * supported by this framework, so this method is deprecated.
	 */
	function patchUpReferences()
	{
		// Hack for PHP4 compatibility
		// Due to the indescribable brain-death that is PHP4's object reference semantics,
		// we have to resort to this method to try and ensure consistency within the form's 
		// constituent objects. Oh, well.
		
		if (substr(phpVersion(), 0, 2) == "4.")
		{
			foreach(array_keys($this->renderers) as $renderer)
			{
				$this->renderers[$renderer]->parent =& $this;
			}
			foreach(array_keys($this->overrides) as $override)
			{
				$this->overrides[$override]["renderer"]->parent =& $this;
			}
		}
	}
}



?>