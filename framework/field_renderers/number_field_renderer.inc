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
 * Field renderer for numeric data fields. Renders as a text box
 * with numerically masked input (user cannot enter non-numeric characters).
 * 
 * Allow negative defaults to false; if a field should allow negative numbers, 
 * calling script should add the following line:
 * $form->getRenderer("field_name")->allowNegative = true;
 */
class NumberFieldRenderer extends FieldRenderer
{
	// Whether user can enter a negative number
	var $allowNegative = false;
	var $size = 15;
	var $onKeyUp; // javascript callback function
	var $disable = false; // alternative to readonly; field can be reenabled or set thru javascript
	
	function NumberFieldRenderer(&$parent)
	{
		$this->FieldRenderer($parent);
	}
	
	function renderField($field)
	{
		$this->_startField($field);
		
		$disable = ($this->disable) ? "disabled='disabled'" : "";
		$onKeyUp = ($this->onKeyUp) ? " onkeyup='{$this->onKeyUp}()'" : "";
		// JDG - maskInput function doesn't recognize boolean var value
		// translate to 0 or 1
		$allowneg = ($this->allowNegative) ? 1 : 0;
		echo "<input id='{$this->parent->id}_$field' type='text' name='$field' 
		value='".$this->parent->data->get($field)."' 
		size='{$this->size}' onkeypress='return maskInput(event, $allowneg);' $disable $onKeyUp/>";

		$this->_endField($field);
	}

	function renderReadOnly($field)
	{
		echo "<tr>\n";
		$this->_printLabel($field);
		echo "<td";
		if ($this->parent->valueCSS) echo " class='{$this->parent->valueCSS}'";
		echo ">".$this->parent->data->get($field)."</td>\n";		
		echo "</tr>\n";	
	}
	
	// if users enter negatives where they shouldn't (any position other than first,
	// strip them out
	function preProcess($field)
	{
		$value= $_POST[$field];
		
		$value = str_replace(",", "", $value);
		
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

?>