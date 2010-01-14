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

require_once realpath(dirname(__FILE__))."/../field_renderers.inc";

/**
 * Field renderer for handling one-to-many relationships that
 * are implemented through cross-reference tables. The postProcess()
 * method is used to automatically update the cross-reference 
 * records when the form is submitted.
 */
class CrossReferenceSelectFieldRenderer extends FieldRenderer
{
	var $relatedClass;
	var $constraint;
	var $xrefClass;
	var $nameField;
	var $field;
	var $label;
	var $options;
	
	function CrossReferenceSelectFieldRenderer(&$form, $field, $label, $options, $nameField, $xrefClass)
	{
		$this->field = $field;
		$this->label = $label;
		$this->options = $options;
		$this->nameField = $nameField;
		$this->xrefClass = $xrefClass;
		
		$this->FieldRenderer($form);
		$form->add($this);
	}
	
	function setSize($width, $height)
	{
		$this->width = $width;
		$this->height = $height;
	}
	
	function renderField($field = "")
	{
		if (count($this->options) == 0) return;
		
		$field = $this->field;
		
		$this->_startField($field);
		
		$obj = $this->parent->data;
		$pk = $obj->primary_key;
		
		$rc = $this->options[0];
		
		$itempk = $rc->primary_key;
		
		$selected = ($obj->$pk != "") ? indexedQuery($this->xrefClass, "WHERE {$pk}={$obj->$pk}", $rc->primary_key) : array();
		
		if ($this->width && $this->height)
		{
			echo "<div style='width: {$this->width}px; height: {$this->height}px; border: solid 1px black; overflow: auto'>\n";
		}
		
		foreach($this->options as $item)
		{
			$p = $item->$itempk;
			
			echo "<input type='checkbox' class='checkbox' name='{$field}[$p]' value='1'";
			if (array_key_exists($item->$itempk, $selected))
			{
				echo " checked";
			}
			echo ">&nbsp;";
			echo $this->formatName($item, $this->nameField);
			echo "<br/>";
		}

		if ($this->width && $this->height)
		{
			echo "</div>\n";
		}
		
		$this->_endField($field);
	}
	
	function renderReadOnly($field = "")
	{
		if (count($this->options) == 0) return;
		
		$obj = $this->parent->data;
		$pk = $obj->primary_key;
		
		if ($obj->$pk == "") return;
		
		$optpk = $this->options[0]->primary_key;
		
		$items = indexedQuery($this->xrefClass, "WHERE {$pk}={$obj->$pk}", $optpk);
		
		echo "<tr>\n";
		$this->_printLabel($this->label);

		echo "<td";
		if ($this->parent->valueCSS) echo " class='{$this->parent->valueCSS}'";
		echo ">";

		foreach($this->options as $item)
		{
			if (array_key_exists($item->$optpk, $items))
			{
				echo $this->formatName($item, $this->nameField)."<br/>";
			}
		}
		
		echo "</td></tr>\n";
	}
	
	function postProcess($field = "")
	{
		$obj = $this->parent->data;
		$pk = $obj->primary_key;
		
		$optpk = $this->options[0]->primary_key;
		
		// Clear out current relations
		
		$xref = new $this->xrefClass;
		$xref->delete("WHERE {$pk}={$obj->$pk}");
		
		if (isset($_POST[$this->field]))
		{
			foreach($_POST[$this->field] as $optionpk => $dummy)
			{
				$xref = new $this->xrefClass;
				$xref->$pk = $obj->$pk;
				$xref->$optpk = $optionpk;
				$xref->save();
			}
		}
	}
}


?>