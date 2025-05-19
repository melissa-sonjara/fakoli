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
 * CompoundSelectFieldRenderer allows you to provide a consolidated
 * grouped dropdown selector that processes selection for multiple 
 * fields. 
 * @author andy
 *
 */
class CompoundSelectFieldRenderer extends FieldRenderer
{
	var $options;
	var $onChange; ///< optional selection change handler
	var $width;
	var $max_chars;
	
	var $defaultBlank = true; ///< whether the drop down includes a blank as the default
	var $defaultText = "Choose One";
	var $emptyValue = 0; ///< The value to which the non-selected fields should be set
	
	function __construct(&$form, $field, $label, $onChange)
	{
		$this->options = array();
		$this->onChange = $onChange;

		parent::__construct($form);
		if ($form->data->hasField($field))
		{
			$form->override($field, $label, $this);
			if ($this->options == null)
			{
				$opt = $form->data->distinctValues($field, true);
				if (count($opt) > 0)
				{
					$this->options = array_combine($opt, $opt);
				}
				else
				{
					$this->options = array();
				}
			}
		}
		else
		{
			$form->add($this, $field);
			// JDG 7/22/2011 - allow "additional" fields to override label
			$form->overrides[$field]['label'] = $label;
		}	
	}
	
	function add($title, $field, $items, $format)
	{
		$this->options[$field] = array("title"	=> 	$title,
									   "items"	=>	$items,
									   "format"	=>	$format);
		
		$this->parent->hide($field);
		
		return $this;
	}
	
	function renderField($field)
	{
		$this->_startField($field);
			
		$current = $this->getValue();
		
		if($this->width)
			$style = "style='width: {$this->width}'";
		echo "<select $style name='$field' id='{$this->parent->id}_{$field}'";
			
		if ($this->onChange != "")
		echo " onchange='$this->onChange(this)'";
		elseif($this->addEntry)
		echo " onchange='onChange_{$this->parent->id}_{$field}(this)'";
			
		echo ">\n";
	
		if ($this->defaultBlank)
		{
			echo "<option value=''></option>";
		}
		
		foreach($this->options as $subField => $group)
		{							

			$groupName = $group["title"];
			
			echo "<optgroup label=\"$groupName\">\n";
			
			foreach($group["items"] as $item)
			{
				$value = "$subField:".$item->get($subField);
				$name = $item->format($group["format"]);
				
				$name = htmlSafe($name);
				$trunced = $this->max_chars ? ellipsis($name, $this->max_chars, true) : $name;
		
				if($current == $value)
				{
					$selected = " selected";
				}
				else
					$selected = "";

				$optionTitle = ($name != $trunced) ? " title='$name'" : "";
	
				echo "<option$optionTitle value='".htmlSafe($value)."'$selected>$trunced</option>\n";
			}
			
			echo "</optgroup>\n";
		}
		
		echo "</select>\n";
	
		$this->_endField($field);
	}
		
	
	function getValue()
	{
		$obj = $this->parent->data;
		
		foreach(array_keys($this->options) as $field)
		{
			$value = $obj->get($field);
			if ($value)
			{
				return "$field:$value";
			}
		}
		
		return "";
	}
	
	function preProcess($field = "")
	{		
		$obj = $this->parent->data;
		
		$value = ($this->parent->method == "POST") ? $_POST[$field] : $_GET[$field];
		
		list($targetField, $value) = explode(":", $value);
		
		foreach(array_keys($this->options) as $f)
		{
			if ($targetField == $f)
			{
				$obj->set($f, $value);
			}
			else
			{
				$obj->set($f, $this->emptyValue);
			}
		}
	}
}

/**@}*/?>