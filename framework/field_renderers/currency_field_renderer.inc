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

require_once realpath(dirname(__FILE__))."/../field_renderers.inc";

/**
 * Field renderer for currency data fields. Provides a two decimal place
 * display and some simple numeric validation as default behavior.
 */
class CurrencyFieldRenderer extends FieldRenderer
{
	var $disable = false; // alternative to readonly; field can be reenabled thru javascript
	
	function CurrencyFieldRenderer(&$parent)
	{
		$this->FieldRenderer($parent);
	}
	
	function renderScript($field)
	{
		//TODO: Add Numeric handling script from ICT-in-Education Toolkit
	}
	
	function renderField($field)
	{
		$value = htmlspecialchars($this->parent->data->get($field), ENT_QUOTES, 'UTF-8');
		
		// display with commas between thousands
		$value = number_format($value, 2);
		$disable = ($this->disable) ? "disabled='disabled'" : "";
	
		$this->_startField($field);

		echo "$&nbsp;<input id='{$this->parent->id}_{$field}' type='text' name='$field' value='{$value}' $disable size='20'/>";

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
		echo "<tr>\n";
		$this->_printLabel($field);
		echo "<td";
		if ($this->parent->valueCSS) echo " class='{$this->parent->valueCSS}'";
		echo ">$".number_format($this->parent->data->get($field), 2)."</td>\n";		
		echo "</tr>\n";			
	}
	
	// allow user to enter commas if they wish and strip them out before saving
	function preProcess($field)
	{
		$value= $_POST[$field];
		$this->parent->data->set($field, str_replace(",", "", $value));
	}
	
	function addValidatorsToForm($field, $required = false)
	{
		$label = isset($this->label) ? $this->label : $this->parent->prettifyFieldName($field);
		
		if($required)
		{
			$this->parent->validator->add(new RequiredCurrencyValidator($field, $label));
		}
		
	}
	
	
}
