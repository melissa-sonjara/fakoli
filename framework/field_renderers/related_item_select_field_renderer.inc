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
 * Field renderer for foreign key data fields. Renders as a dropdown list
 * of related items. Optionally, you can specify that the user be allowed
 * to create new items in the related table during entry.
 */
class RelatedItemSelectFieldRenderer extends FieldRenderer
{
	var $items; 				///< The related item choices (populated by the control)
	var $default;				///< The default selected value
	var $field;					///< The field name
	var $nameField;				///< The name field or field format
	var $valueField;			///< The value field
	var $allowAdd;				///< True to allow items to be added when entering the form
	var $relatedClass;			///< The DataItem class of the related items
	var $constraint;			///< Constraint to apply when populating the item choice list
	var $max_chars;				///< The maximum number of characters to display in the select choices (0 for no limit)
	var $allowNone;				///< Set to true to provide an option for no selection
	var $templateItem;			///< The template of a DataItem to use when adding items is enabled in the form via the allowAdd parameter
	var $sorted;				///< Set to true to sort the items alphabetically (independent of any sort order in the constraint field)
	var $otherField;			///< Name of a field in the form to allow the user to specify specific "Other" value that is not in the item list
	var $onChange;				///< Name of a Javascript handler function to be called when the selection changes
	var $width;					///< Width of the control (in CSS format)
	var $defaultText;			///< Text to display in the 'no selection' option when allowNone is enabled
	var $defaultReadOnlyText;	///< Text to display when the control is read-only and the is no selected value;
	
	static $optionsCache = array();
	
/**
 * Create drop down list of item names in one table based on the key
 * stored in another.
 * 
 * Example: "User" table has a field called "school_key". In the datamodel,
 * "User" class is defined in the $relations attribute as having a related 
 * table, "Schools". The login form allows the user to specify their school 
 * with a drop down. In this case, the $field is "school_key", the 
 * $relatedClass is "Schools".
 * 
 * @param AutoForm object $form : form where this drop down will be displayed
 * 
 * @param String $field : 	"value field" - field name of the dataitem (table) displayed on the form 
 * 							where selection is be stored
 * 
 * @param String $label : display name for the drop down list
 * 
 * @param dataitem object $relatedClass : name of related dataitem class, defined
 * 							in $relations attribute of the dataitem instance displayed 
 * 							in the form, that provides the full text description 
 * 							for display in the drop down
 *  
 * @param String $constraint : SQL constraint on query in the form "WHERE..." 
 * 								and/or "ORDER BY...". 
 * 
 * @param String $nameField	 : "select name" - specifies the fields in the related table to be displayed
 * 							   in the drop down. This String can include more than one field
 * 							   (e.g., "{f_name} {l_name}")
 * 
 * @param String $valueField : the name of the field in the related table that needs to
 * 								match the field in the table displayed on the form. This 
 * 								field name will often be identical to the $field name 
 * 								string.
 * 
 * @param Boolean $allowAdd : optional, whether the drop down list is editable
 * 
 * @param Boolean $allowNone : optional, whether the drop down list can have a blank
 * 
 * @param Integer $maxChars : optional, width of drop down
 * 
 * @return RelatedItemSelectFieldRenderer
 */
	function RelatedItemSelectFieldRenderer(&$form, $field, $label, $relatedClass, $constraint, $nameField, $valueField = "", $allowAdd = false, $allowNone = false, $maxChars = 80)
	{	
		$this->relatedClass = $relatedClass;
		$this->constraint = $constraint;
		$this->field = $field;
		$this->nameField = $nameField;
		$this->valueField = $valueField;
		$this->allowAdd = $allowAdd;
		$this->allowNone = $allowNone;
		$this->max_chars = $maxChars;
		$this->templateItem = null;
		$this->sorted = false;
		
		$this->FieldRenderer($form);
		if ($form->data->hasField($field))
		{
			$form->override($field, $label, $this);
		}
		else
		{
			$form->add($this, $field);
			// JDG 8/14/2011 - allow "additional" fields to override label
			$form->overrides[$field]['label'] = $label;
		}
	}
	
	function allowOther($field)
	{
		$this->allowAdd = false; // Cannot have 'add' and 'other' at the same time
		$this->otherField = $field;
	}
	
	function renderField($field)
	{
		$this->getRelatedItems();

		if($this->width)
				$style = "style='width: {$this->width}'";
		$this->_startField($field);
		echo "<select $style id='{$this->parent->id}_{$field}' name='$field'";
		
		if ($this->onChange)
		{
			echo " onchange='{$this->onChange}(this);'";
		}
		echo ">\n";

		$vals = array();
		foreach($this->items as $item)
		{
			$name = $this->formatName($item, $this->nameField);
			$valueField = ($this->valueField != "") ? $this->valueField : $item->getPrimaryKey();
			$value = $item->get($valueField);
			
			$vals[$name] = $value;
		}
		
		if ($this->sorted)
		{
			ksort($vals, SORT_LOCALE_STRING);
		}
		
		$current = $this->parent->data->get($field);
		
		foreach($vals as $name => $value)
		{

			$name = htmlSafe($name);
			$trunced = $this->max_chars ? ellipsis($name, $this->max_chars, true) : $name;
			//trace("valueField: $valueField", 3);
			
			echo "<option value='{$value}'";
			
			if ($trunced != $name) echo " title='$name'";
			
			/*
			 * If this form is new and there is a default, then
			 * set the default as selected. If this form is
			 * retrieved or loaded from a saved record, then
			 * set selected to the field value of that record
			 */			
			if (($current != NULL) AND ($value == $current)) 
				echo " selected";
			elseif (($current == NULL) AND ($value != NULL) AND ($value == $default))
				echo " selected";
			echo ">$trunced</option>\n";			
		}
		echo "</select>";
		
		if ($this->allowAdd)
		{
			echo "<span style='white-space: nowrap'> or add new: <input type='text' name='autoform_add_$field' value='' size='30'></span>";
		}
		else if ($this->otherField)
		{
			$otherField = $this->otherField;
			$filter = $this->parent->data->getFilter();
			
			$value = $this->parent->data->get($otherField);
			
			$pkField = $this->parent->data->getPrimaryKey();
			if ($this->parent->data->get($pkField) && 
				$filter && 
				$filter->isExcluded($this->otherField))
			{
				$obj = clone($this->parent->data);
				$obj->setFilter(new InclusionFilter($obj->getPrimaryKey(), $this->otherField));
				$obj->select();
				$value = $obj->get($otherField);
			}
			else
			{
				$value = $this->parent->data->get($otherField);
			}
			
			echo "<span style='white-space: nowrap'> if other, please specify: <input type='text' name='{$otherField}' value='{$value}' size='30'/></span>";
		}
		$this->_endField($field);
	}
	
	function renderSearchField($field, $mode)
	{
		if ($mode != "equal") return;
		
		$this->getRelatedItems();
		
		$this->_startField($field);
		
		if($this->width)
			$style = "style='width: {$this->width}'";
		
		$this->_printLabel($field);
		echo "<select name='{$field}:$mode' $style>\n";
		
		//AJG: Don't add double empty slots when allowNone is true
		if (!$this->allowNone) 
		{
			echo "<option value=''></option>\n";
		}
		
		$value = $this->parent->params->get($field, $mode);
		
		foreach($this->items as $item)
		{
			$valueField = ($this->valueField != "") ? $this->valueField : $item->getPrimaryKey();
			$name = $this->formatName($item, $this->nameField);

			echo "<option value='{$item->get($valueField)}'";
			if ($item->get($valueField) == $value) echo " selected";
			echo ">".ellipsis($name, $this->max_chars)."</option>\n";			
		}
		echo "</select>";
		
		$this->_endField($field);
	}
	
	function renderReadOnly($field)
	{	
		$item = $this->getSelectedItem($field);
		
		$this->_startField($field);
		
		trace("RelatedItemSelectFieldRenderer renderReadOnly name field is {$this->nameField} field is $field and item is {$item->role}", 3);
		if (!$item->getPrimaryKeyValue())
		{
			trace("Using default readonly text '{$this->defaultReadOnlyText}'", 3);
			$name = $this->defaultReadOnlyText;
		}
		else
		{
			$name = $this->formatName($item, $this->nameField);
		}
		
		if (!$this->parent->readOnlyForm)
		{
			$valueField = ($this->valueField != "") ? $this->valueField : $item->getPrimaryKey();
			$value = $item->get($valueField);
			echo "<input type='hidden' id='{$this->parent->id}_{$field}' name='$field' value='$value'/>";
		}
		
		echo $name;
		
		$this->_endField($field);
		
	}

	function getRelatedItems($readonly = false)
	{
		$cacheKey = "$this->relatedClass, $this->constraint";
		
		if (array_key_exists($cacheKey, RelatedItemSelectFieldRenderer::$optionsCache))
		{
			$this->items = RelatedItemSelectFieldRenderer::$optionsCache[$cacheKey];
		}
		else
		{
			$this->items = query($this->relatedClass, $this->constraint);
			RelatedItemSelectFieldRenderer::$optionsCache[$cacheKey] = $this->items;
		}
		
		if ($this->allowNone)
		{
			$none = new $this->relatedClass;
			if (is_string($this->allowNone))
			{
				$nameField = $this->nameField;
				if ($readonly)
				{
					$none->set($nameField, $this->defaultReadOnlyText);
				}
				else if ($this->defaultText)
				{
					$none->set($nameField, $this->defaultText);
				}
				else
				{
					$none->set($nameField, $this->allowNone);
				}
			}
			$this->items = array_merge(array($none), $this->items);
		}
		return $this->items;
	}

	function getSelectedItem($field)
	{
		$item = new $this->relatedClass;
		$valueField = $this->valueField ? $this->valueField : $item->getPrimaryKey();
		$value = $this->parent->data->get($field);
		
		try
		{
			$item = Query::create($this->relatedClass, "WHERE {$valueField}=:v")
						 ->bind(":v", $value)
						 ->executeSingle();
		}
		catch(DataNotFoundException $e)
		{
			$item = new $this->relatedClass;
		}
		
		return $item;
	}
	
	function preProcess($field = "")
	{
		trace("RelatedItemSelectFieldRenderer::process()", 3);
		
		$item = new $this->relatedClass;
		
		if ($this->otherField)
		{
			$filter = $this->parent->data->getFilter();
			
			// Ensure that the 'other' field value gets written
			if ($filter)
			{
				$filter->includeField($this->otherField);
				$otherField = $this->otherField;
				$this->parent->data->set($otherField, isset($_POST[$otherField]) ? $_POST[$otherField] : "");
			}
		}
		
		if (!$this->allowAdd) return;
		$valueField = $valueField ? $this->valueField : $item->getPrimaryKey();
		$field = "autoform_add_$valueField";
		$val = $_POST[$field];
		
		trace("Checking for value '$val' in field $field", 3);
		
		if ($val)
		{
			foreach($this->items as $item)
			{
				if ($val == $this->formatName($item, $this->nameField))
				{
					$this->data->set($valueField, $item->get($valueField));
					return;
				}
			}
			
			$obj = ($this->templateItem) ? $this->templateItem : new $this->relatedClass;
			$pk = $obj->getPrimaryKey();
			
			$nameField = $this->nameField;
			
			$obj->set($nameField, $val);
			$obj->save();
			$this->parent->data->set($valueField, $obj->get($pk));
		}
	}
}

?>