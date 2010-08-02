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
	var $showSelectAll;
	
	function CrossReferenceSelectFieldRenderer(&$form, $field, $label, $options, $nameField, $xrefClass)
	{
		$this->field = $field;
		$this->label = $label;
		$this->options = $options;
		$this->nameField = $nameField;
		$this->xrefClass = $xrefClass;
		$this->showSelectAll = false;
		
		$this->FieldRenderer($form);
		$form->add($this);
	}
	
	function setSize($width, $height)
	{
		$this->width = $width;
		$this->height = $height;
	}
	
	function renderScript($field)
	{
		if ($this->showSelectAll)
		{
?>
<script type="text/javascript">

function <?echo $this->field ?>_toggleSelectAll(link)
{
	var value = (link.innerHTML == "Select All");

	var inputs = document.getElementsByTagName("INPUT");

	for(var i = 0; i < inputs.length; ++i)
	{
		var input = inputs[i];
		if (input.attributes['id'] && input.attributes['id'].value.indexOf('checkbox_<?echo $this->field?>') == 0)
		{
			input.checked = value;
		}
	}

	link.innerHTML = (value) ? "Deselect All" : "Select All";
	return false;
}

</script>
<?
		}
	}
	
	function getMatchingPK($xref, $obj)
	{
		$pks = $obj->getPrimaryKeyList();
		foreach($pks as $pk)
		{
			if ($xref->hasField($pk)) return $pk;
		}
		
		throw new DataItemException("No matching foreign key in xref table");
	}
	
	function renderField($field = "")
	{
		if (count($this->options) == 0) return;
		
		$field = $this->field;
		
		$this->_startField($field);
		
		$obj = $this->parent->data;

		$rc = $this->options[0];

		$xref = new $this->xrefClass;
		
		$itempk = $this->getMatchingPK($xref, $rc);
		$pk = $this->getMatchingPK($xref, $obj);
		
		$selected = ($obj->get($pk) != "") ? indexedQuery($this->xrefClass, "WHERE {$pk}={$obj->get($pk)}", $rc->getPrimaryKey()) : array();
		
		if ($this->showSelectAll)
		{
			echo "<a href='#' onclick='return {$field}_toggleSelectAll(this);'>Select All</a><br/>";
		}
		if ($this->width && $this->height)
		{
			echo "<div style='width: {$this->width}px; height: {$this->height}px; border: solid 1px black; overflow: auto'>\n";
		}
		
		$countOptions = count($this->options);
		foreach($this->options as $item)
		{
			$p = $item->$itempk;
			
			echo "<input type='checkbox' class='checkbox' name='{$field}[$p]' id='checkbox_{$field}_$p' value='1'";
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

		// Save this count to loop through checkboxes on required validation
		echo "<input type=\"hidden\" name=\"count_{$field}\" value=\"$countOptions\">";		
		
		$this->_endField($field);
	}
	
	function renderReadOnly($field = "")
	{
		if (count($this->options) == 0) return;
		
		$xref = new $this->xrefClass;
		
		$obj = $this->parent->data;
		$pk = $this->getMatchingPK($xref, $obj);
		
		if ($obj->get($pk) == "") return;
		
		$optpk = $this->getMatchingPK($xref, $this->options[0]);
		
		$items = indexedQuery($this->xrefClass, "WHERE {$pk}=".$obj->get($pk), $optpk);
		
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
		$xref = new $this->xrefClass;
		
		$obj = $this->parent->data;
		$pk =  $this->getMatchingPK($xref, $obj);
		
		$optpk = $this->getMatchingPK($xref, $this->options[0]);
		
		// Clear out current relations
		
		$xref = new $this->xrefClass;
		$xref->delete("WHERE {$pk}=".$obj->get($pk));
		
		if (isset($_POST[$this->field]))
		{
			foreach($_POST[$this->field] as $optionpk => $dummy)
			{
				$xref = new $this->xrefClass;
				$xref->set($pk, $obj->get($pk));
				$xref->set($optpk, $optionpk);
				$xref->save();
			}
		}
	}
}


?>