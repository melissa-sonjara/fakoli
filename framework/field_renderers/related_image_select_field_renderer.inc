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
 * Field renderer for selected related items from a list of images. 
 * Renders as a custom drop-down-list style control, implemented via Javascript.
 */
class RelatedImageSelectFieldRenderer extends FieldRenderer
{
	var $items;
	var $default;
	var $field;
	var $imageField;
	var $valueField;
	var $relatedClass;
	var $constraint;	
	var $onChange;
	
	function RelatedImageSelectFieldRenderer(&$form, $field, $label, $relatedClass, $constraint, $imageField, $valueField = "")
	{
		$this->relatedClass = $relatedClass;
		$this->constraint = $constraint;
		$this->field = $field;
		$this->imageField = $imageField;
		$this->valueField = $valueField;
		$this->allowAdd = $allowAdd;
		$this->allowNone = $allowNone;
		$this->max_chars = $maxChars;
		
		$this->getRelatedItems();
		
		$this->FieldRenderer($form);
		$form->override($field, $label, $this);
	}
	
	function renderField($field)
	{		
		
		$obj =& $this->parent->data;
		$imgField = $this->imageField;
		
		if ($obj->$field)
		{
			$related = $obj->getRelated($this->relatedClass, $field);
			$valueField = ($this->valueField != "") ? $this->valueField : $related->primary_key;
			//trace("valueField: $valueField", 3);
			$img = "<img src='{$related->$imgField}'/>";
		}
		else
		{
			$img = "Click to Select";
		}

		$this->_startField($field);
		echo "<input type='hidden' name='$field'  value='{$this->parent->data->$field}'><button name='{$field}_button' class='{$this->parent->buttonCSS}' onclick='{$field}_toggle(); return false;'>$img</button>";
		
		echo "<div style='display: none; position: absolute' id='{$field}_options'>";
	
		$items = $this->getRelatedItems();
		
		foreach($items as $item)
		{
			$valueField = ($this->valueField != "") ? $this->valueField : $item->primary_key;
			
			//trace("valueField: $valueField imageField: $imgField", 3);
			echo "<img style='cursor: pointer; float: left' src='{$item->$imgField}' onclick='{$field}_select(\"{$item->$valueField}\", \"{$item->$imgField}\"); return false;'/>";
		}
		
		echo "</div>";
		$this->_endField($field);
		
	}
	
	function renderScript($field)
	{
?>
<script type='text/javascript'>
/* <![CDATA[ */
	function <?echo $field ?>_select(value, image)
	{
		var form = document.getElementById('<?echo $this->parent->id ?>');
		var ctrl = form['<?echo $field ?>'];
		var button = form['<?echo $field ?>_button'];
		ctrl.value = value;
		button.innerHTML = "<img src='" + image + "'/>";
		<?echo $field ?>_hide();
<? if ($this->onChange)
{ ?>
		<?echo $this->onChange ?>(value, image);
<?
}
 ?>
	}
	
	function <?echo $field ?>_toggle()
	{
		var div = document.getElementById('<?echo $field ?>_options');
		if (div.style.display == 'none')
		{
			div.style.display = 'block';
		}
		else
		{
			div.style.display = 'none';
		}
	}
	
	function <?echo $field ?>_hide()
	{
		var div = document.getElementById('<?echo $field ?>_options');
		div.style.display = 'none';
	}
			
// ]]>
</script>
<?
	}
	function renderSearchField($field, $mode)
	{
		/*if ($mode != "equal") return;
		
		echo "<tr>\n";
		$this->_printLabel($field);
		echo "<td><select name='{$field}:$mode'>\n";
		echo "<option value=''></option>\n";
		$valueField = ($this->valueField != "") ? $this->valueField : $item->primary_key;
		//trace("valueField: $valueField", 3);
		
		$value = $this->parent->params->get($field, $mode);
		
		foreach($this->items as $item)
		{
			$name = $this->formatName($item, $this->nameField);

			echo "<option value='{$item->$valueField}'";
			if ($item->$valueField == $value) echo " selected";
			echo ">".ellipsis($name, $this->max_chars)."</option>\n";			
		}
		echo "</select>";*/
	}
	
	function renderReadOnly($field)
	{		
		echo "<tr>\n";
		$this->_printLabel($field);
		echo "<td";
		if ($this->parent->valueCSS) echo " class='{$this->parent->valueCSS}'";
		echo ">";
		
		/*foreach($this->items as $item)
		{
			$valueField = $this->valueField ? $this->valueField : $item->primary_key;
			
			if ($this->parent->data->$field == $item->$valueField)
			{
				$name = $this->formatName($item, $this->nameField);
				echo $name;
				break;
			}
		}*/
		
		echo "</td>\n";
		echo "</tr>\n";
	}

	function getRelatedItems()
	{
		$this->items = query($this->relatedClass, $this->constraint);
		if ($this->allowNone)
		{
			$this->items = array_merge(array(new $this->relatedClass), $this->items);
		}
		return $this->items;
	}

	
	function preProcess($field = "")
	{
		/*trace("RelatedItemSelectFieldRenderer::process()", 3);
		
		$item = new $this->relatedClass;
		
		if (!$this->allowAdd) return;
		$valueField = $valueField ? $this->valueField : $item->primary_key;
		$field = "autoform_add_$valueField";
		$val = $_POST[$field];
		
		trace("Checking for value '$val' in field $field", 3);
		
		if ($val)
		{
			foreach($this->items as $item)
			{
				if ($val == $this->formatName($item, $this->nameField))
				{
					$this->data->$valueField = $item->$valueField;
					return;
				}
			}
			
			$obj = new $this->relatedClass;
			$pk = $obj->primary_key;
			
			$nameField = $this->nameField;
			
			$obj->$nameField = $val;
			$obj->save();
			$this->parent->data->$valueField = $obj->$pk;
		}*/
	}
}

?>