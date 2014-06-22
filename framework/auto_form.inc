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

require_once realpath(dirname(__FILE__)."/field_renderers.inc");
require_once realpath(dirname(__FILE__)."/auto_form_layout.inc");
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
	var $componentPath ="/fakoli";
	var $buttonAlignment = "left";		///< Default Form button alignment
	var $passwordEncryptor = "";		///< Password Encryption method name

	var $capitalizationMode = "word";	///< Capitalization to be used for labels. Can be 'lower', 'upper', 'first' or 'word'.

	var $allowDelete = false;			///< If true, the form will display a delete button if editing an existing item.

	var $method = "POST";				///< HTTP method that will be used to submit the form.
	var $action = "";					///< Action URL for submitting the form. Generally this can be left blank to submit back to the same page.

	var $renderers = array();			///< The field renderers collection
	var $overrides = array();			///< The field overrides collection
	var $hidden = array();				///< The hidden fields collection
	var $additional = array();			///< The additional (pseudo) fields collection
	var $additional_at_top = false;		// whether additional fields are rendered above the datamodel's field list; if false, then rendered below
	var $readonly = array();			///< The read-only fields collection
	var $annotations = array();			///< The field annotations collection
	var $buttons = array();				///< The custom buttons collection

	var $validator;						///< The ValidationEngine for this form
	var $useLinkSubmit = false;			///< Flag to indicate whether the submit button for the form should be rendered as a link (not common).

	var $HTMLEditorHideStyleBar = false;	///< Flag to indicate that Rich Text Editors should be displayed with or without the style editing toolbar.
	var $HTMLEditorDefaultWidth = "540px";	///< Default width for Rich Text Editor controls.
	var $HTMLEditorDefaultHeight = "200px";	///< Default height for Rich Text Editor controls.

	var $passwordEnterLabel = "Enter";		///< Text for Password Enter label
	var $passwordConfirmLabel = "Confirm";	///< Text for Password Confirm label
	
	var $onSaveComplete = null;	///< Callback event handler that is fired after the AutoForm has finished saving data to the database.

	var $onDelete = "";					///< delete method name
    var $customSaveHandler = null;		///< Use this field to override the default save behavior of your form.
	var $markRequiredFields = false;	///< Set to true to add an asterisk after required field labels.
	var $onFormatLabel = null; 			///< Label Processing hook
    var $buttons_at_top = false;
    var $ajaxSubmitHandler = false;		///< JavaScript Callback for AJAX Submit mode - called on success
    var $ajaxFailureHandler = false;	///< JavaScript Callback for AJAX Submit mode - called on failure
    var $onSubmitHandler = null;        ///< Javascript Callback for user-defined onsubmit hook
    var $required = array(); // list of required fields
    var $requiredFieldsText = "* indicates required field";
	var $interstitialMessage = null;	///< Specifies the message to display in an interstitial while the form is being submitted
    var $fields;	///< Local field cache

    var $groups = array(); 				///< List of field groups
    var $collapsibleGroups = array(); 	///< List of collapsible groups and their default states
    var $groupDescriptions = array();	///< List of group descriptions
    
    var $groupDescriptionCSS = "group_description";	///< CSS class for group description blocks
    
    var $readOnlyForm;  ///< Specifies whether the entire form is read-only
    var $subordinate;	///< Specifies whether this form is a subordinate member of a composite form
    var $hideEmptyFields = false; ///< Specifies whether to hide empty fields when displaying a read-only form
    
    var $layout; ///< Layout handler object
    var $default_layout = "table";
    
    // When adding new renderers, must also update
    // DataItem $dataTypeRendererMap
    static $fieldRendererMap = array (
    	String		=>	StringFieldRenderer,
    	HTML		=>	HTMLFieldRenderer,
		Date		=>	DateFieldRenderer,
    	DateTime	=>	DateTimeFieldRenderer,
    	Number		=>	NumberFieldRenderer,
    	Currency	=>	CurrencyFieldRenderer,
		Boolean		=>	BooleanFieldRenderer,
		TimeZone	=>	TimezoneFieldRenderer,
		Password	=>	PasswordFieldRenderer,
		Text		=>	TextFieldRenderer,
		Time		=>	TimeFieldRenderer,
		Rating		=>	RatingFieldRenderer,
		ZipCode		=>	ZipCodeFieldRenderer,
		PhoneNumber	=>	PhoneNumberFieldRenderer,
    	URL			=>	URLFieldRenderer
		);	///< Provides the mapping from Type identifier to FieldRenderer
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

		$this->subordinate = false;
		
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

		$this->fields = $this->data->getFields();
		
		$filter = $this->data->getFilter();
		
		// Add in a default renderer for each field
		foreach($this->fields as $field => $type)
		{
			if ($filter && $filter->isExcluded($field)) continue;
			
			$this->renderers[$field] = $this->createRenderer($field);
		}
		
		$aliases = $this->data->getFieldAliases();
		// Read in default alias definitions from DataItem
		if ($aliases)
		{
			foreach($aliases as $field => $alias)
			{
				$this->alias($field, $alias);
			}
		}
		
		$pks = $this->data->getPrimaryKeyList();
		foreach($pks as $p)
		{
			$this->hide($p);
		}
		
		$this->layout = AutoFormLayout::create($this->default_layout, $this);
	}
	
	/**
	 * Returns the target data item for the form
	 */
	function getData()
	{
		return $this->data;
	}

	function setDataSet($params)
	{
		$this->dataSet = $params;
	}
	
	/**
	 * Override the default layout manager
	 * 
	 */
	function setLayout($layout)
	{
		$this->layout = $layout;
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
		if (!array_key_exists($field, $this->fields)) return null;

		$type = $this->fields[$field];
		
		if ($type == "Timestamp") return null; // No field renderer required for this type
		
		if (array_key_exists($type, AutoForm::$fieldRendererMap))
		{
			return new AutoForm::$fieldRendererMap[$type]($this, $field);
		}
		
		throw new FakoliException("AutoForm::createRenderer(): Unknown type '$type'");
	}

	/**
	 * Registers a custom type-to-renderer mapping.
	 * @param string $type the type identifier
	 * @param string $rendererClass the class name of the renderer to handle fields of this type
	 */
	static function registerFieldRendererClass($type, $rendererClass)
	{
		AutoForm::$fieldRendererMap[$type] = $rendererClass;
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
		$filter = $this->data->getFilter();
		
		foreach(array_keys($this->fields) as $field)
		{
			if ($filter && $filter->isExcluded($field)) continue;
			if ($this->readonly[$field]) continue; // No need to validate readonly fields
			
			$renderer = $this->getRenderer($field);
			if ($renderer)
			{
				// JDG 9/19/2010 - let each renderer supply its own validator
				$required = (array_search($field, $this->required) === FALSE) ? false : true;
				$renderer->addValidatorsToForm($field, $required);
			}
		}

		foreach($this->additional as $r)
		{
			// JDG 5/2012 - check if additional field is required
			$required = (array_search($r['field'], $this->required) === FALSE) ? false : true;
			$r['renderer']->addValidatorsToForm($r['field'], $required);
		}
	}

	/**
	 * Retrieve the validation engine for this AutoForm.
	 */
	function getValidationEngine()
	{
		return $this->validator;
	}
	
	/**
	 * Set this AutoForm to be a subordinate form within a CompositeAutoForm.
	 */
	function makeSubordinate()
	{
		$this->subordinate = true;
	}
	
	function configureValidators()
	{
		if ($this->validatorsConfigured) return;
		
		$this->addDefaultValidators();

		foreach($this->validator->validators as $v)
		{
			$v->title = (!$v->title) ? $this->prettifyFieldName($v->field) : $v->title;
			$v->readOnly = $this->isReadOnly($v->field) || $this->isHidden($v->field);
		}
		
		$this->validatorsConfigured = true;
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

		$this->configureValidators();
		
		$obj =& $this->data;
		$filter = $obj->getFilter();
		$pk = $obj->getPrimaryKey();
		
		$script = "";
		foreach(array_keys($this->fields) as $field)
		{
			if ($field != $pk && !($filter && $filter->isExcluded($field)))
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
		
		echo "\tif (validate_{$this->id} != null && !validate_{$this->id}(form)) return false;\n";

		foreach(array_keys($this->fields) as $field)
		{
			if ($field != $pk && !($filter && $filter->isExcluded($field)))
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

		if ($this->onSubmitHandler)
		{
			echo "\t if (!{$this->onSubmitHandler}(form)) return false;\n;";
		}
		
		if ($this->interstitialMessage)
		{
			echo "\tinterstitial('".jsSafe($this->interstitialMessage)."');\n;";
		}
		
		echo "\treturn true;\n}\n\n";
		
		echo "\twindow.addEvent('domready', function() {\n";
		
		if ($this->ajaxSubmitHandler)
		{
?>
document.id('<?echo $this->id?>').iFrameFormRequest({'onRequest': function() { return onSubmit<?echo $this->id?>(document.id('<?echo $this->id?>')); }, 'onComplete': <?echo $this->ajaxSubmitHandler; ?>, 'onFailure': <?echo $this->ajaxFailureHandler; ?>});
<?

		}

		// Add collapsible group handlers
		
		foreach($this->collapsibleGroups as $group => $toggle)
		{
			$groupID = $this->id . "_" . codify($group) . "_group";
			$toggleID = $this->id . "_" . $toggle;
			
			echo "\tdocument.id('$toggleID').addEvent('click', function(e) { if (this.checked) {  document.id('$groupID').setStyle('opacity', 0).removeClass('collapsed').addClass('expanded').fade('in'); } else { document.id('$groupID').removeClass('expanded').addClass('collapsed'); } });\n";
		}
		echo "\t});\n";
		echo "</script>\n";
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
		else // JDG 2/13/2011 get renderer from additional field set
		{
			foreach($this->additional as $r)
			{
				$renderer = $r['renderer'];
				$rField = $r['field'];
				if(!$rField)
					$rField = $renderer->field;
				if($rField == $field)
					return $renderer;
			}					
		}
		
		trace("AutoForm::getRenderer(): Unknown field: $field", 2);
		return null;
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
	 * Check whether the specified field is hidden in the form.
	 * @param string $field the name of the field
	 * @return boolean true if the field has been hidden, false otherwise.
	 */
	function isHidden($field)
	{
		return array_key_exists($field, $this->hidden) ? $this->hidden[$field] : false;
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
	 * Define a field grouping
	 * @param string $name the legend to use for the group
	 * 
	 */
	function group($name)
	{
		$group = (array_key_exists($name, $this->groups)) ? $this->groups[$name] : array();
		for($i = 1; $i < func_num_args(); ++$i)
		{
			$group[] = func_get_arg($i);
		}
		
		$this->groups[$name] = $group;
		return $group;
	}
	
	/**
	 * Add a description block to a group. This will be displayed within the group,
	 * after then title (if displayed) but before any fields.
	 * @param string $name the name of the group
	 * @param string $description the description to display for that group
	 */
	function groupDescription($name, $description)
	{
		$this->groupDescriptions[$name] = $description;
	}
	
	/**
	 * Retrieves the group description for the specified group.
	 */
	function getGroupDescription($name)
	{
		if (array_key_exists($name, $this->groupDescriptions)) return $this->groupDescriptions[$name];
		return "";
	}
	
	/**
	 * Allows a group to be shown or hidden based on the value of another control
	 * @param string $name the name of the group
	 * @param string $toggleControl the field name of the toggling control
	 */
	function makeGroupCollapsible($name, $toggleControl)
	{
		$this->collapsibleGroups[$name] = $toggleControl;
	}
	
	/**
	 * Remove the specified field grouping from the form. Any fields within the group
	 * will be displayed ungrouped if they are not hidden.
	 * @param string $name the legend of the group
	 */
	function removeGroup($name)
	{
		unset($this->groups[$name]);
	}
	
	/**
	 * Use this method to hide the labels for the specified fields in the form.
	 */
	function hideLabel()
	{
		foreach(func_get_args() as $naked)
		{
			$this->getRenderer($naked)->hideLabel = true;
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
	 * Determine whether the specified field is being displayed read-only
	 * @param string $field the name of the field
	 * @return boolean true if read-only, false otherwise
	 */
	function isReadOnly($field)
	{
		return $this->readonly[$field];
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
			if ($this->data->hasField($required) || $this->hasAdditional($required))
			{
				array_push($this->required, $required);
			}
		}
	}
	
	/**
	 * Check if this field exists as an additional field
	 */
	function hasAdditional($field)
	{
		foreach($this->additional as $r)
		{
			$renderer = $r['renderer'];
			$rField = $r['field'];
			if($rField == $field)
				return true;
		}

		return $false;
	}

	/**
	 * Determine whether the specified field is a required field for the form.
	 * @param $field the field to check
	 * @return boolean true if the field is required to be filled in by the user, false if it is an optional field.
	 */
	function isRequired($field)
	{
		return (array_search($field, $this->required) !== FALSE) ? true : false;
	}

	/**
	 * Make a field required if a given checkbox is checked.
	 * @param string $field the field to be validated
	 * @param string $checkbox the checkbox controlling whether the field is required
	 * @param string $message the message to display if validation fails.
	 */
	function requiredIfChecked($field, $checkbox, $fieldTitle = "", $checkboxTitle = "", $message = "")
	{
		if ($this->data->hasField($field))
		{
			$this->validator->add(new RequiredIfCheckedValidator($field, $checkbox, 
																 $fieldTitle ? $fieldTitle : $this->prettifyFieldName($field), 
																 $checkboxTitle ? $checkboxTitle : $this->prettifyFieldName($checkbox), $message));
		}
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
	 * @param string $field
	 * @param string, Date, or DateTime $min
	 * @param string, Date or DateTime $max
	 * @param string $message
	 * 
	 * Min can be either a static date min that the field's value
	 * must be set to or the name of a field whose value will be
	 * compared to the given field.
	 * 
	 * E.g., start_date must be no earlier than today
	 * or start_date must be no earlier than end_date
	 */
	function dateRange($field, $min = "", $max = "", $message = null)
	{
		if ($this->data->hasField($field))
		{
			$this->validator->add(new DateRangeValidator($field, $this->prettifyFieldName($field), $min, $max, $message));
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

	function uniqueWithConstraint($field, $message, $constraint = "")
	{
		if ($this->data->hasField($field))
		{
			$this->validator->add(new UniqueValidator($field, $this->prettifyFieldName($field), $this->data, $message, $constraint));
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
	 * @param string $caseSensitive specifies whether the match is case sensitive
	 */
	function match($field, $title, $match, $matchTitle, $caseSensitive = true)
	{
		$this->validator->add(new MatchValidator($field, $title, $match, $matchTitle, $caseSensitive));
	}

	/**
	 * Adds a password validator to the specified field.
	 * 
	 * @param string $field The name of the field.
	 * @param string The title of the field
	 */
	function password($field, $title)
	{
		$this->validator->add(new PasswordValidator($field, $title, $this->id));
	}

	/**
	 * Disable autocomplete for the selected fields. If no fields are specified, 
	 * autocomplete is disabled for all the fields in the form.
	 */
	function disableAutoComplete()
	{
		$numArgs = func_num_args();
		
		if ($numArgs == 0)
		{
			foreach($this->data->getFields() as $field => $type)
			{
				if ($this->data->hasField($field))
				{
					$this->getRenderer($field)->autocomplete = false;
				}
			}
		}
		else
		{
			for($i = 0; $i < $numArgs; ++$i)
			{
				if ($this->data->hasField($field))
				{
					$this->getRenderer($field)->autocomplete = false;
				}
			}
		}
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
	 * Put the form into AJAX submission mode. You must supply a Javascript function to call
	 * once the submission is complete.
	 * @param string $success the Javascript callback function for success
	 * @param string $failure the Javascript callback function for failure
	 */
	function ajaxSubmit($success, $failure = null)
	{
		if ($failure == null)
		{
			$failure = "function() {document.id('{$form->id}_error').set('text','Failed to communicate with server'); }";
		}
		
		$this->ajaxSubmitHandler = $success;
		$this->ajaxFailureHandler = $failure;
	}
	
	/**
	 * Determine if this form is the one that was submitted. Useful for managing pages
	 * with multiple forms.
	 */
	function matchesSubmitted()
	{
		return (isset($_POST["#FORMID#"]) && $_POST["#FORMID#"] == $this->id);
	}

	/**
	 * Renders the form to HTML.
	 *
	 */
	function drawForm()
	{
		if ($this->readOnlyForm) return $this->drawReadOnly();
		
		$obj =& $this->data;
		$pk = $obj->getPrimaryKey();

		$submitLabel = $this->submitLabel;
		if ($submitLabel == "")
		{
			$submitLabel = ($obj->get($pk) != "" && $obj->get($pk) != 0) ? "Update ".$obj->prettifyClassName() : "Add ".$obj->prettifyClassName();
		}

		//echo "<!--\n";
		//print_r($this);
		//echo "\n-->\n";
		
		if (!$this->subordinate)
		{
			echo "<form id='{$this->id}' method='{$this->method}' action='{$this->action}' enctype='multipart/form-data'";
			if (!$this->ajaxSubmitHandler) echo " onsubmit='return onSubmit{$this->id}(this);'";
			echo ">\n";
		}
		
		if ($this->id)
		{
			echo "<input type='hidden' name='#FORMID#' value='{$this->id}'/>";
		}
		
		$csrfToken = $_SESSION["csrfToken"];
		if (!$csrfToken)
		{
			$csrfToken = plainGUID();
			$_SESSION["csrfToken"] = $csrfToken;
		}
		
		echo "<input type='hidden' name='#SESSIONTIE#' value='{$csrfToken}'/>\n";
		
		foreach(array_keys($this->hidden) as $hidden)
		{
			echo "<input id='{$this->id}_{$hidden}' type='hidden' name='$hidden' value='".htmlSafe($obj->get($hidden))."'/>\n";
		}

		if ($this->layout->externalErrorBox) $this->layout->errorBox();
		
		$this->layout->startUngrouped();
		
		if ($this->buttons_at_top && !$this->subordinate) 
		{
			$this->layout->startButtonLine();
			
			$this->drawSubmitButtons($obj, $pk, $submitLabel);

			$this->drawButtons();

			$this->layout->endButtonLine();
		}

		if (!$this->layout->externalErrorBox) $this->layout->errorBox();
		
		if ($this->markRequiredFields && $this->validator->hasRequiredFields())
		{
			$this->layout->requiredFields($this->requiredFieldsText);
		}

		$this->renderAllFields($obj);
					
		if (!$this->subordinate)
		{		
			$this->layout->startButtonLine();

			$this->drawSubmitButtons($obj, $pk, $submitLabel);
			$this->drawButtons();
	
			$this->layout->endButtonLine();
		}

		$this->layout->endUngrouped();
		
		$this->layout->finalizeLayout();
		
		if (!$this->subordinate)
		{
			echo "</form>\n";
		}
		
	}
	
	/*
	 * JDG 12/17/2010 - break out from drawForm into into function 
	 */
	function drawSubmitButtons($obj, $pk, $submitLabel)
	{
		if ($this->useLinkSubmit)
		{
			echo "<a class='{$this->buttonCSS}' name='Submit' onclick='if (onSubmit{$this->id}(document.forms.{$this->id})) return document.forms.{$this->id}.submit(); else return false;'>$submitLabel</a>";
		}
		else
		{
			echo "<input type='submit' class='{$this->buttonCSS}' name='Submit' value='$submitLabel' ondblclick='return false;'/>";
		}

		if ($obj->get($pk) && $this->allowDelete)
		{
			$deleteLabel = ($this->deleteLabel) ? $this->deleteLabel : "Delete this ".$obj->prettifyClassName();
				
			echo "&nbsp;&nbsp;&nbsp;&nbsp;<input type='submit' class='{$this->buttonCSS}' name='{$this->id}_delete'
		      value='$deleteLabel'
		      onclick='return confirm(\"Are you sure you want to delete this ".$obj->prettifyClassName()."?\");'/>";
		}	
	}
	
	function renderAllFields($obj)
	{
		$this->renderedFields = array();
		
		if($this->additional_at_top)
			$this->renderAdditionalFields($obj);	
		$this->renderFields($obj);
		if(!$this->additional_at_top)
			$this->renderAdditionalFields($obj);		
	}
	
	/*
	 * JDG 9/14/2010
	 * Break out this function to facilitate inheritence 
	 * when fields needs custom layout.
	 */
	function renderFields($obj)
	{
		$filter = $obj->getFilter();
		
		if (count($this->groups) > 0)
		{
			$this->layout->endUngrouped();
			
			foreach($this->groups as $legend => $fields)
			{
				$collapsible = array_key_exists($legend, $this->collapsibleGroups);
				if ($collapsible)
				{
					$cf = $this->collapsibleGroups[$legend];
					$collapsed = !$this->data->$cf;
				}
				
				$this->layout->startGroup($legend, $collapsible, $collapsed);
				
				foreach($fields as $field)
				{
					if ($field != $pk && !array_key_exists($field, $this->hidden) && !array_key_exists($field, $this->renderedFields) && !($filter && $filter->isExcluded($field)))
					{
						$this->renderOneField($field);
					}					
				}
				
				$this->layout->endGroup();
			}
			
			$this->layout->startUngrouped();
			
		}
		
		foreach(array_keys($this->fields) as $field)
		{
			if ($field != $pk && !array_key_exists($field, $this->hidden) && !array_key_exists($field, $this->renderedFields) && !($filter && $filter->isExcluded($field)))
			{
				$this->renderOneField($field);
			}
		}
	}

	function renderOneField($field, $renderScript = false)
	{
		$renderer = $this->getRenderer($field);
			
		if ($renderer)
		{
			if ($this->readonly[$field] || $this->readOnlyForm)
			{
				if (!$this->hideEmptyFields || $this->data->get($field))
				{
					$renderer->renderReadOnly($field);
				}
			}
			else
			{
				if ($renderScript)
				{
					$renderer->renderScript($field);
				}
				$renderer->renderField($field);
			}
			
			$this->renderedFields[$field] = true;
		}
	}
	
	/*
	 * JDG 12/17/2010 - break out into function to allow option
	 * to render additional fields at the top.
	 */
	function renderAdditionalFields($obj)
	{
		foreach($this->additional as $r)
		{
			$renderer = $r['renderer'];
			// JDG 5/24/11 - fix get field name
			$field = $r['field'];
			if(!$field)
				$field = $renderer->field;
		
			if(array_key_exists($field, $this->renderedFields))
				continue;
					
			if ($this->readonly[$field] || $this->readOnlyForm)
			{
				$renderer->renderReadOnly($field);
			}
			else
			{
				$renderer->renderField($field);
			}
		}		
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

	/**
	 * Draws the form in read-only mode. This is most often used to display
	 * the details for a DataItem on a preview or details screen.
	 */
	function drawReadOnly()
	{
		$this->readOnlyForm = true;
		
		$this->layout->startUngrouped();
		
		$obj =& $this->data;
		
		$this->renderAllFields($obj);
		
		$this->layout->endUngrouped();	
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

		$this->configureValidators();

		$obj =& $this->data;
		$obj->fromPOST();
		
		$filter = $obj->getFilter();
		$pk = $obj->getPrimaryKey();
		
		if (isset($_POST["#FORMID#"]))
		{
			$this->id = $_POST["#FORMID#"];
		}

		if (!isset($_POST["#SESSIONTIE#"]) || $_POST["#SESSIONTIE#"] != $_SESSION["csrfToken"])
		{
			throw new FakoliException("Invalid Session Token");
		}
		
		if ($_POST["{$this->id}_delete"])
		{
			if(!$this->onDelete)
			{
				$obj->delete(); // no callback function defined
			}
			else
			{
				call_user_func_array($this->onDelete, array($obj));	
			}
			$this->deleted = true;
			return true;
		}

		$this->msg = $this->validator->validate();
		if ($this->msg != "") return false;

		$this->preProcessFields($pk, $filter);
	
		if ($this->customSaveHandler)
		{
			if (!call_user_func_array($this->customSaveHandler, array($this))) return false;
		}
		else
		{
			$obj->save();
		}
		
		$this->postProcessFields($pk, $filter);
		
		// onComplete event is fired once all processing has been completed

		if ($this->onSaveComplete)
		{
			call_user_func_array($this->onSaveComplete, array($this));
		}

		return true;
	}

	function preProcessFields($pk, $filter)
	{
		foreach (array_keys($this->fields) as $field)
		{
			if ($field != $pk  && !array_key_exists($field, $this->hidden) &&
			!($filter && $filter->isExcluded($field)))
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
	}
	
	function postProcessFields($pk, $filter)
	{
		foreach (array_keys($this->fields) as $field)
		{
			if ($field != $pk  && !array_key_exists($field, $this->hidden) &&
			!($filter && $filter->isExcluded($field)))
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