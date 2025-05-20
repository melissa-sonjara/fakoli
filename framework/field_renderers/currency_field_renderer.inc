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
 * Field renderer for currency data fields. Provides a two decimal place
 * display and some simple numeric validation as default behavior.
 */
class CurrencyFieldRenderer extends FieldRenderer
{
	var $allowNegative = false;	///< Whether user can enter a negative number
	var $autocomplete = true;	///< Whether the browser is allowed to show autocomplete for this field
	var $disable = false; 	///< alternative to readonly; field can be reenabled thru javascript
	var $onChange = ""; 	///< optional javascript callback on data input change
	var $onKeyUp; 			///< optional javascript callback on key release within field
	var $template = 2; 		///< how to format readonly - 2 decimal places  
	var $size = 20;			///< size of the text field to be displayed (in characters)
	var $static = false;		///< Render the field as a static (readonly) value, that is visible, non-editable and the value is submitted when the form is submitted.
	
	function __construct(&$parent)
	{
		parent::__construct($parent);
	}
	
	function renderScript($field)
	{
		//TODO: Add Numeric handling script from ICT-in-Education Toolkit
	}
	
	function renderField($field)
	{
		$value = htmlspecialchars($this->parent->data->get($field), ENT_QUOTES, 'UTF-8');
		
		$value = floatval($value);
		// display with commas between thousands
		$value = $this->format($field);
		$disable = ($this->disable) ? "disabled='disabled'" : "";
		$readonly = ($this->static) ? " readonly='readonly'" : "";
		$allowneg = ($this->allowNegative) ? 1 : 0;
		$autocomplete = ($this->autocomplete) ? "" : " autocomplete='off' ";
		$onchange = ($this->onChange) ? "onchange='$this->onChange(this)'" : "";		
		$onKeyUp = ($this->onKeyUp) ? " onkeyup='{$this->onKeyUp}(this)'" : "";
		
		$this->_startField($field);

		echo "$&nbsp;<input id='{$this->parent->id}_{$field}' 
		type='text' name='$field' value='{$value}' $autocomplete onkeypress='return maskInput(event, $allowneg);'
		$onchange $onKeyUp $disable $readonly size='{$this->size}'/>";

		$this->_endField($field);
	}
	
	function renderSearchField($field, $mode = "equal")
	{
		if ($mode == "range")
		{
			$from = htmlspecialchars($this->parent->params->get($field, "from"), ENT_QUOTES, 'UTF-8');
			$to =  htmlspecialchars($this->parent->params->get($field, "to"), ENT_QUOTES, 'UTF-8');
			
			echo "<tr>\n";
			$this->_printLabel($field);
			echo "<td>From <input type='text' name='$field:from' value='{$from}' size='20'> to <input type='text' name='$field:to' value='{$to}' size='20'></td>\n";		
			echo "</tr>\n";
		}
		else
		{
			$value = htmlspecialchars($this->parent->params->get($field, $mode), ENT_QUOTES, 'UTF-8');
			echo "<tr>\n";
			$this->_printLabel($field);
			echo "<td><input type='text' name='$field:$mode' value='{$value}' size='20'></td>\n";		
			echo "</tr>\n";
		}		
	}
		
	function renderReadOnly($field)
	{
		$this->_startField($field);

		echo "$". $this->format($field);		

		$this->_endField($field);
	}
	
	// allow user to enter commas if they wish and strip them out before saving
	function preProcess($field = "")
	{
		if ($this->parent->readOnlyForm || $this->parent->isReadOnly($field) || array_key_exists($field, $this->parent->hidden)) return "";
			
		$value= $_POST[$field];
		$value =  str_replace(",", "", $value);
		$this->parent->data->set($field, $value);
		$_POST[$field] = $value;
	}
	
	function addValidatorsToForm($field, $required = false)
	{
		$label = isset($this->label) ? $this->label : $this->parent->prettifyFieldName($field);
		
		if($required)
		{
			$this->parent->validator->add(new RequiredCurrencyValidator($field, $label));
		}
		
	}
	
	function renderOnSubmitHandler($field)
	{
		// JDG 3/2/2011., no js for readonly field
		if ($this->parent->isReadOnly($field)) return "";
		
		echo "\tdocument.id('{$this->parent->id}_{$field}').value = rawNumber(document.id('{$this->parent->id}_{$field}').value);\n";
	}
	
	function format($field)
	{
		return CurrencyTypeRenderer::format($this->parent->data->get($field), $this->template);
	}
	
}

/**
 * Use Currency3 if you want to have 3 decimal places displayed.
 * @author andy
 *
 */
class Currency3FieldRenderer extends CurrencyFieldRenderer
{
	function __construct(&$parent)
	{
		parent::__construct($parent);
		$this->template = 3;
	}
}

/**@}*/?>
