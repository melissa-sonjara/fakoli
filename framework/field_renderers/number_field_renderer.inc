<?php
/**\addtogroup fieldrenderers
 * @{
 */

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

require_once realpath(dirname(__FILE__))."/../field_renderers.inc";

/**
 * Field renderer for numeric data fields. Renders as a text box
 * with numerically masked input (user cannot enter non-numeric characters).
 * 
 * Allow negative defaults to false; if a field should allow negative numbers, 
 * calling script should add the following line:
 * $form->getRenderer("field_name")->allowNegative = true;
 */
class NumberFieldRenderer extends FieldRenderer
{
	
	var $allowNegative = false;	///< Whether user can enter a negative number
	var $autocomplete = true;	///< Whether the browser is allowed to show autocomplete for this field
	var $size = 15;				///< Size of the displayed control in characters
	var $onKeyUp; 				///< optional javascript callback function for key up events
	var $disable = false; 		///< alternative to readonly; field can be reenabled or set thru javascript
	var $onChange = ""; 		///< optional javascript callback on data input change
	var $units = "";			///< units text to display
	var $unitsPosition = "";	///< whether to display units before or after the control
	var $blankZero = false;		///< Leave field blank if value is zero
	var $control = "text";		///< The HTML control type to use when rendering. This gets set to "number" if a range is specified.
	var $static = false;		///< Render the field as a static (readonly) value, that is visible, non-editable and the value is submitted when the form is submitted.
	
	function __construct(&$parent, $field = null)
	{
		parent::__construct($parent);
		if ($field && !$parent->data->hasField($field))
		{
			$parent->add($this, $field);
		}
	}
	
	function addValidatorsToForm($field, $required = false)
	{
		parent::addValidatorsToForm($field);
		
		if ($this->control == "number")
		{
			$label = isset($this->label) ? $this->label : $this->parent->prettifyFieldName($field);
		
			$this->parent->validator->add(new RangeValidator($field, $label, $this->min, $this->max));
		}
	}
	
	/**
	 * Sets the displayed units and their position
	 * @param string $units
	 * @param string $position
	 */
	function setUnits($units, $position)
	{
		$this->units = $units;
		$this->unitsPosition = $position;
		return $this;
	}
	
	/**
	 * Specifies that the field should be rendered with a "number" HTML control with the supplied range and step.
	 * 
	 * @param mixed $min the minimum value for the field
	 * @param mixed $max the maximum value for the field
	 * @param mixed $step the amount to step when the spinner up/down buttons are clicked
	 */
	function setRange($min, $max, $step = 1)
	{
		$this->min = min($max, $min);
		$this->max = max($max, $min);
		$this->step = $step;
		$this->control = "number";
		
		return $this;
	}
	
	function renderField($field)
	{
		$this->_startField($field);
		
		$readonly = ($this->static) ? " readonly='readonly'" : "";
		$disable = ($this->disable) ? "disabled='disabled'" : "";
		$onKeyUp = ($this->onKeyUp) ? " onkeyup='{$this->onKeyUp}(this)'" : "";
		// JDG - maskInput function doesn't recognize boolean var value
		// translate to 0 or 1
		$allowneg = ($this->allowNegative) ? 1 : 0;
		$autocomplete = ($this->autocomplete) ? "" : " autocomplete='off' ";
		$onchange = ($this->onChange) ? "onchange='{$this->onChange}(this)'" : "";		
		
		if ($this->unitsPosition == "before") echo $this->units." ";
		
		$value = $this->parent->data->get($field);
		if ($value == 0 && $this->blankZero)
		{
			$value = "";
		}
		
		if ($this->control == "number")
		{
			$control = "number";
			$rangeAttrs = " min='{$this->min}' max='{$this->max}' step='{$this->step}'";
		}
		else 
		{
			$control = "text";
			$rangeAttrs = "";
		}
		echo "<input id='{$this->parent->id}_$field' type='{$control}' style='width: auto' name='$field' 
		value='".$value."' $rangeAttrs $autocomplete 
		size='{$this->size}' onkeypress='return maskInput(event, $allowneg);' $disable $static $onchange $onKeyUp/>";
		
		if ($this->unitsPosition == "after") echo " ".$this->units;
		
		$this->_endField($field);
	}

	function renderReadOnly($field)
	{
		$this->_startField($field);

		if ($this->unitsPosition == "before") echo $this->units." ";
		
		echo $this->parent->data->get($field);

		if ($this->unitsPosition == "after") echo " ".$this->units;
		
		$this->_endField($field);
	}
	
	// if users enter negatives where they shouldn't (any position other than first,
	// strip them out
	function preProcess($field = "")
	{
		$current = $this->parent->data->get($field);
				
		$value = str_replace(",", "", $current);
		
		if($this->allowNegative)
		{			
			if(substr($value, 0,1) == '-')
				$value = '-' . str_replace("-", "", $value);
			else
				$value = str_replace("-", "", $value);
		}
		$this->parent->data->$field = $value;
	}	
}

/**@}*/?>