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
 * Field renderer for data fields that must be displayed as a list
 * of choices. Renders as a set of radio buttons from which the user
 * can choose only one response.
 */
class RadioButtonFieldRenderer extends FieldRenderer
{
	var $options;
	var $horizontal = false;
	var $onChange = ""; // optional callback on data input change
	var $disable = false; // alternative to readonly; field can be reenabled thru javascript
	
	
	function RadioButtonFieldRenderer(&$form, $field, $label, $options)
	{
		$this->options = $options;
				
		$this->FieldRenderer($form);
		if ($form->data->hasField($field))
		{
			$form->override($field, $label, $this);
		}
		else
		{
			$form->add($this, $field);
			$form->alias($field, $label);
		}
	}

	function addValidatorsToForm($field, $required = false)
	{
		$label = isset($this->label) ? $this->label : $this->parent->prettifyFieldName($field);
		
		// JDG 2/2012 - changed from tree validator expects a div and does not match the rendering of this type
		if($required)
		{
			$this->parent->validator->add(new RequiredRadioButtonValidator($field, $label));
		}
		
	}
	
	
	function renderField($field)
	{
		if($this->horizontal)
			$this->colspan = 2;
		$this->_startField($field);
		
		$current = $this->parent->data->get($field);
		
		foreach($this->options as $value => $name)
		{
			$selected = ($current == $value) ? " checked" : "";
			$onchange = ($this->onChange) ? "onclick='$this->onChange(this)'" : "";
			$disable = ($this->disable) ? "disabled='disabled'" : "";
			
			// JDG 3/34/10, add border: none to correct IE8 display issue
			echo "<input id='{$this->parent->id}_$field' style='border: none' type='radio' name='$field' value='$value'$selected $onchange $disable/>&nbsp;$name";
			echo ($this->horizontal) ? "&nbsp;&nbsp;" : "<br/>";
		}
		
		$this->_endField($field);
	}
	
	/*
	 * JDG, 3/12/10, fixed rendering style from select field
	 * to radio button.
	 */
	function renderSearchField($field, $mode)
	{
		if ($mode != "equal") return;
		
		$searchValue = $this->parent->params->get($field, $mode);
		echo "<tr>\n";
		$this->_printLabel($field);
		echo "<td>\n";
		foreach($this->options as $value => $name)
		{
			$selected = ($searchValue == $value) ? " checked" : "";
			echo "<input style='border: none' type='radio' name='$field:$mode' value='$value'$selected>$name</option>";
			echo ($this->horizontal) ? "&nbsp;&nbsp;" : "<br/>";
		}
		echo "</td></tr>\n";
	}
	
	
	function renderReadOnly($field)
	{
		echo "<tr>\n";
		$this->_printLabel($field);
		echo "<td";
		if ($this->parent->valueCSS) echo " class='{$this->parent->valueCSS}'";
		echo ">";
		if (array_key_exists($this->parent->data->get($field), $this->options))
		{
			echo $this->options[$this->parent->data->get($field)];
		}
		else
		{
			echo "Not specified";
		}
		echo "</td></tr>\n";
	}
}


?>