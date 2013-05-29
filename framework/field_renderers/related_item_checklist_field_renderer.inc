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
 * are implemented through a back-reference table. The postProcess()
 * method is used to automatically update the table
 * records when the form is submitted.
 * 
 * e.g., AutoForm renders instance of ProductCategory containing
 * a RelatedItemCheckListFieldRenderer additional field "products"
 * to select the products that belong to the category. Selections
 * are saved to the product table's field product_category_id.
 */
class RelatedItemCheckListFieldRenderer extends FieldRenderer
{
	var $relatedClass;
	var $constraint;
	var $nameField;
	var $field;
	var $label;
	var $options;
	var $groupBy;
	
	function __construct(&$form, $field, $label, $relatedClass, $constraint, $nameField)
	{
		$this->field = $field;
		$this->label = $label;
		$this->nameField = $nameField;
		$this->relatedClass = $relatedClass;
		$this->constraint = $constraint;
		$this->options = $this->getRelatedItems();
		
		$this->FieldRenderer($form);
		
		if ($form->data->hasField($field))
		{
			$form->override($field, $label, $this);
		}
		else
		{
			$form->add($this, $field);
			$form->overrides[$field]['label'] = $label;
		}
	}

	function setSize($width, $height)
	{
		$this->width = $width;
		$this->height = $height;
	}
			
	function groupBy($groupBy, $format = "")
	{
		$this->groupBy = $groupBy;
		$this->groupByFormat = $format;
	}
	
	/**
	 * Find the field in the DataItem obj displayed in the AutoForm
	 * that exists in the back-reference class
	 * e.g., if ProductCategory AutoForm's obj, and products are the option items
	 * displayed, then product_category_id would exist in the Product table
	 * 
	 * @param unknown $relatedClass
	 * @param unknown $obj
	 * @throws DataItemException
	 * @return unknown
	 */
	function getMatchingPK($relatedClass, $obj)
	{
		$pks = $obj->getPrimaryKeyList();
		foreach($pks as $pk)
		{
			if ($relatedClass->hasField($pk)) return $pk;
		}
		
		throw new DataItemException("No matching foreign key in related table {$relatedClass->table}");
	}
		
	function renderField($field = "")
	{
		if (count($this->options) == 0) return;
			
		$field = $this->field;
			
		$this->_startField($field);
			
		$obj = $this->parent->data;
	
		if (is_array($this->groupBy) && count($this->groupBy) > 0)
		{
			$keys = array_keys($this->options);
		}
	
		$itempk = $this->getMatchingPK($this->options[0], $obj);
		$related_pk = $this->options[0]->getPrimaryKey();
		$selected = $this->getSelected($obj, $itempk, $related_pk);
		
		if ($this->width && $this->height)
		{			
			echo "<div id='{$this->parent->id}_{$field}_container' class='scrollbox{$resizable}{$dropdown}' style='width: {$this->width}px; height: {$this->height}px; overflow: auto'>\n";
		}
		else
		{
			echo "<div class='selectbox'>\n";
		}		
		$countOptions = count($this->options);
			
		if (is_array($this->groupBy) && count($this->groupBy) > 0)
		{
			$gpk = $this->groupBy[0]->getPrimaryKey();
							
			foreach($this->groupBy as $group)
			{
				if (array_key_exists($group->get($gpk), $this->options))
				{
					echo $group->format($this->groupByFormat);
					$this->renderCheckboxes($field, $this->options[$group->get($gpk)], $related_pk, $selected); 
				}
			}
		}
		else if (is_string($this->groupBy))
		{
			$options = regroupList($this->options, $this->groupBy);

			foreach($options as $group => $choices)
			{
				echo "<br/><h4>$group</h4>";
				$this->renderCheckboxes($field, $choices, $related_pk, $selected);
			}
		}
		else
		{
			$this->renderCheckBoxes($field, $this->options, $related_pk, $selected);  
		}
			
		echo "</div>\n";
			
		// Save this count to loop through checkboxes on required validation
		echo "<input type=\"hidden\" name=\"count_{$field}\" value=\"$countOptions\">";		
			
		$this->_endField($field);
	}
	
	function getRelatedItems()
	{
		return Query::create($this->relatedClass, $this->constraint)->execute();
	}
	
	/**
	 * 
	 * @param obj $obj DataItem class in AutoForm
	 * @param String $itempk back-reference key of the related class
	 * @param String $pk - primary key of the back-reference related table
	 * @return array of objects of related class
	 */
	function getSelected($obj, $itempk, $pk)
	{
		return IndexedQuery::create($this->relatedClass, "WHERE {$itempk}=:{$itempk}", $pk)
			->bind("{$itempk}", $obj->get($itempk))
			->execute();
	}	
			
	function renderCheckboxes($field, $items, $pk, $selected)
	{
		foreach($items as $item)
		{
			$item_id = $item->$pk;
				
			echo "<input type='checkbox' class='checkbox' name='{$field}[$item_id]' id='checkbox_{$field}_$item_id' value='1'";
			if (array_key_exists($item_id, $selected))
			{
				echo " checked";
			}
			echo ">&nbsp;";
	
			echo "<label for='checkbox_{$field}_$item_id'>".$this->formatName($item, $this->nameField)."</label>";
			echo "<br/>";
		}
	}
		
	function renderReadOnly($field = "")
	{
		if (count($this->options) == 0) return;
			
		$obj = $this->parent->data;
		$pk = $relatedClassObj->getPrimaryKey();
		$selected = $this->getSelected($obj, $pk);
		
		$this->_startField($field);
		
		foreach($this->options as $item)
		{
			if (array_key_exists($item->$pk, $selected))
			{
				echo $this->formatName($item, $this->nameField)."<br/>";
			}
		}
			
		$this->_endField($field);
	}
		
	function postProcess($field = "")
	{
		if (count($this->options) == 0) return;

		$option_pk = $this->options[0]->getPrimaryKey();
		$obj_pk = $this->parent->data->getPrimaryKey();
		$obj_id = $this->parent->data->$obj_pk;
		
		$items = reindexList($this->options, $option_pk);
		
		$selected = $_POST[$this->field];
	
		/**
		 * Save each checked item to the back-reference
		 * related table
		 */
		foreach($selected as $selected_pk => $value)
		{
			if(!array_key_exists($selected_pk, $items)) continue;
			
			$item = $items[$selected_pk];
			$item->filter = new InclusionFilter($obj_pk);
			$item->$obj_pk= $obj_id;
			$item->save();
		} 
		
		/**
		 * Remove the link from the back-reference related table
		 * to the form's data obj if no longer selected.
		 */
		foreach($items as $item_id => $item)
		{
			if($item->$obj_pk == $obj_id && !array_key_exists($item->$option_pk, $selected))
			{
				$item->filter = new InclusionFilter($obj_pk);
				$item->$obj_pk = 0;
				$item->save();
			}
		}
	}	
}