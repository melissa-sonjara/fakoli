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
 * Field renderer for related item data fields. 
 * Renders as a list of radio button options.
 */
class RelatedItemRadioButtonFieldRenderer extends FieldRenderer
{
	var $items;
	var $default;
	var $field;
	var $nameField;
	var $valueField;
	var $relatedClass;
	var $constraint;
	var $horizontal;
	
	function RelatedItemRadioButtonFieldRenderer(&$form, $field, $label, $relatedClass, $constraint, $nameField, $valueField = "")
	{
		$this->relatedClass = $relatedClass;
		$this->constraint = $constraint;
		$this->field = $field;
		$this->nameField = $nameField;
		$this->valueField = $valueField;
		$this->horizontal = true;
		
		$this->getRelatedItems();
		
		$this->FieldRenderer($form);
		$form->override($field, $label, $this);
	}
	
	function getRelatedItems()
	{
		$this->items = query($this->relatedClass, $this->constraint);
		return $this->items;
	}	

	function renderField($field)
	{		
		$this->_startField($field);

		$current = $this->parent->data->get($field);
		
		foreach($this->items as $item)
		{
			$name = $this->formatName($item, $this->nameField);
			$valueField = ($this->valueField != "") ? $this->valueField : $item->getPrimaryKey();
			//trace("valueField: $valueField", 3);
			
			echo "<input type='radio' name='{$field}' value='{$item->$valueField}'";
			if ($item->get($valueField) == $current) echo " selected";
			echo ">&nbsp;$name\n";

			echo ($this->horizontal) ? "&nbsp;&nbsp;&nbsp;" : "<br/>";
		}
	
		$this->_endField($field);
	}

	function renderSearchField($field, $mode)
	{
		if ($mode != "equal") return;
		
		echo "<tr>\n";
		$this->_printLabel($field);
		echo "<td>";
		$valueField = ($this->valueField != "") ? $this->valueField : $item->primary_key;
		//trace("valueField: $valueField", 3);
		
		$value = $this->parent->params->get($field, $mode);
		$current = $this->parent->data->get($field);
		
		foreach($this->items as $item)
		{
			$name = $this->formatName($item, $this->nameField);

			echo "<input type='radio' name='{$field}:$mode' value='{$value}'";
			if ($item->get($valueField) == $current) echo " selected";
			echo ">&nbsp;$name\n";

			echo ($this->horizontal) ? "&nbsp;&nbsp;&nbsp;" : "<br/>";			
		}
		echo "</select>";
	}
	
	function renderReadOnly($field)
	{		
		$this->_startField($field);
				
		foreach($this->items as $item)
		{
			$valueField = $this->valueField ? $this->valueField : $item->getPrimaryKey();
			
			if ($this->parent->data->get($field) == $item->get($valueField))
			{
				$name = $this->formatName($item, $this->nameField);
				echo $name;
				break;
			}
		}	
		$this->_endField($field);
	}	
}

?>