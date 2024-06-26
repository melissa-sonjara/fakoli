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
	var $xrefField;
	var $nameField;
	var $field;
	var $label;
	var $options;
	var $groupBy;	///< Group the checkboxes when displaying. Either contains a field name or an array of grouping DataItems
	var $groupByFormat; ///< Format string to use when generating group headings from grouping objects
	var $resizable = false;	///< Whether the control is resizable (only relevant when the control has been set to a fixed size)
	var $dropdown;	///< True to display the control a multi-select dropdown, false (default) to display inline in the form
	var $dropdownMaxWidth = "340px";	///< Width of the multi-select dropdown (only relevant when the control has been configured to display as a dropdown)
	var $dropdownMessage = "Click to Select Terms";	///< Text to display on dropdown when no items have been selected
	var $showSelectAll = false;	///< Set to true to show a "Select All" toggle
	var $disable = false;	///< Set to true to disable user input for this control
	var $columns = 1;
	
	function CrossReferenceSelectFieldRenderer(&$form, $field, $label, $options, $nameField, $xrefClass, $xrefField = null)
	{
		$this->field = $field;
		$this->label = ($label) ? $label : $form->overrides[$field]['label'];

		$this->options = $options;
		$this->nameField = $nameField;
		$this->xrefClass = $xrefClass;
		$this->xrefField = $xrefField;
		$this->showSelectAll = false;
		
		$this->FieldRenderer($form);
		
		// JDG 7/31/2011 - allow override
		if ($form->data->hasField($field))
		{
			$form->override($field, $label, $this);
		}
		else
		{
			$form->add($this, $field);
			$form->overrides[$field]['label'] = $label;
		}	
		
		if ($form instanceof SearchForm)
		{
			if (count($options))
			{
				$foreignKey = $this->xrefField ? $this->xrefField : $options[0]->getPrimaryKey();
			
				$class = get_class($form->data);
	
				trace("## Adding CrossReferenceSearchParameterHandler for $class via $xrefClass", 3);
				
				$form->params->setHandler($field, new CrossReferenceSearchParameterHandler($class, $xrefClass, $foreignKey));
				$form->setMatchingMode("member", $field);
			}
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
		if (input.attributes['id'] && input.attributes['id'].value.indexOf('<?php echo $this->parent->id?>_checkbox_<?echo $this->field?>') == 0)
		{
			input.checked = value;
		}
	}

<?
	if ($this->dropdown)
	{
?>
		document.id('<?echo $this->parent->id?>_<?echo $field?>_container').multiSelect.selectionChanged();
<?
	}
?>
	link.innerHTML = (value) ? "Deselect All" : "Select All";
	return false;
}

</script>
<?
		}
		

		if ($this->dropdown)
		{
			?>
			<script type='text/javascript'>
			window.addEvent('domready', function()
			{
				new MultiSelect('<?echo $this->parent->id?>_<?echo $field?>_container', {maxWidth: '<?echo $this->dropdownMaxWidth?>', message: '<?echo jsSafe($this->dropdownMessage)?>'});
			});
			</script> 			
			<?
		}
				
	}
	

	function renderSearchScript($field, $mode)
	{
	
		if ($this->dropdown)
		{
			?>
			<script type='text/javascript'>
			window.addEvent('domready', function()
			{
				new MultiSelect('<?echo $this->parent->id?>_<?echo $this->field?>_container', {maxWidth: '<?echo $this->dropdownMaxWidth?>', message: '<?echo jsSafe($this->dropdownMessage)?>'});
			});
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

		if (is_array($this->groupBy) && count($this->groupBy) > 0)
		{
			$keys = array_keys($this->options);
			$rc = $this->options[$keys[0]][0];
		}
		else
		{
			$rc = $this->options[0];
		}

		$xref = new $this->xrefClass;
		
		$itempk = $this->getMatchingPK($xref, $rc);
		$pk = $this->getMatchingPK($xref, $obj);
		
		$selected = $this->getSelected($obj, $rc, $pk);
		
		if ($this->showSelectAll)
		{
			echo "<a class='select_toggle' href='#' onclick='return {$field}_toggleSelectAll(this);'>Select All</a><br/>";
		}
		if ($this->width && $this->height)
		{			
			$resizable = ($this->resizable && !$this->dropdown) ? " resizable": "";
			$dropdown = $this->dropdown ? " multi_select_dropdown_list" : "";
			echo "<div id='{$this->parent->id}_{$field}_container' class='selectbox scrollbox{$resizable}{$dropdown}' style='width: {$this->width}px; height: {$this->height}px; overflow: auto'>\n";
		}
		else
		{
			echo "<div id='{$this->parent->id}_{$field}' class='selectbox'>\n";
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
					$this->renderCheckboxes($field, $this->options[$group->get($gpk)], $itempk, $selected); 
				}
			}
		}
		else if (is_string($this->groupBy))
		{
			$options = regroupList($this->options, $this->groupBy);

			foreach($options as $group => $choices)
			{
				if (is_callable($this->groupByFormat))
				{
					$group = call_user_func($this->groupByFormat, $group);
				}
				echo "<br/><h4>$group</h4>";
				$this->renderCheckboxes($field, $choices, $itempk, $selected);
			}
		}
		else
		{
			$this->renderCheckBoxes($field, $this->options, $itempk, $selected);  
		}
		
		echo "</div>\n";
		
		// Save this count to loop through checkboxes on required validation
		echo "<input type=\"hidden\" name=\"count_{$field}\" value=\"$countOptions\">";		
		
		$this->_endField($field);
	}

	function getSelected($obj, $rc, $pk)
	{
		$itempk = $this->xrefField ? $this->xrefField: $rc->getPrimaryKey();
		return ($obj->get($pk) != "") ? indexedQuery($this->xrefClass, "WHERE {$pk}={$obj->get($pk)}", $itempk) : array();
	}	
	
	
	function renderCheckboxes($field, $items, $itempk, $selected, $mode = null)
	{
		$name = $mode ? "$field:$mode" : $field;
		
		if ($this->columns > 1)
		{
			echo "<table class='layout'><tbody><tr><td>";
			$colheight = ceil(count($items) / $this->columns);
		}
		
		$i = 0; 
		foreach($items as $item)
		{
			++$i;
			
			$p = $item->$itempk;
			$v = $mode ? $p : "1";
				
			$disable = ($this->disable) ? "disabled='disabled'" : "";
			echo "<label for='{$this->parent->id}_checkbox_{$field}_$p'>";
			echo "<input type='checkbox' class='checkbox' name='{$name}[$p]' id='{$this->parent->id}_checkbox_{$field}_$p' $disable value='{$v}'";
			if (array_key_exists($item->$itempk, $selected))
			{
				echo " checked";
			}
			echo "/>";

			echo $this->formatName($item, $this->nameField)."</label>";
			

			if ($this->columns > 1 && ($i % $colheight == 0))
			{
				echo "</td>";
				if ($i < count($items))
				{
					echo "<td>";
				}
			}
		}

		if ($this->columns > 1)
		{
			echo "</td></tr></tbody></table>\n";
			$colheight = ceil(count($items) / $this->columns);
		}
	}

	function renderSearchField($field = "", $mode = "member")
	{
		if (count($this->options) == 0) return;
		
		$field = $this->field;
	
		$this->_startField($field);
	
		$obj = $this->parent->data;
	
		if (is_array($this->groupBy) && count($this->groupBy) > 0)
		{
			$keys = array_keys($this->options);
			$rc = $this->options[$keys[0]][0];
		}
		else
		{
			$rc = $this->options[0];
		}
	
		$xref = new $this->xrefClass;
	
		$itempk = $this->getMatchingPK($xref, $rc);
		$pk = $this->getMatchingPK($xref, $obj);
	
		$selected = $this->getSelected($obj, $rc, $pk);
	
		if ($this->showSelectAll)
		{
			echo "<a href='#' onclick='return {$field}_toggleSelectAll(this);'>Select All</a><br/>";
		}
		if ($this->width && $this->height)
		{
			$resizable = ($this->resizable && !$this->dropdown) ? " resizable": "";
			$dropdown = $this->dropdown ? " multi_select_dropdown_list" : "";
			echo "<div id='{$this->parent->id}_{$field}_container' class='selectbox scrollbox{$resizable}{$dropdown}' style='width: {$this->width}px; height: {$this->height}px; overflow: auto'>\n";
		}
		else
		{
			echo "<div id='{$this->parent->id}_{$field}' class='selectbox'>\n";
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
					$this->renderCheckboxes($field, $this->options[$group->get($gpk)], $itempk, $selected, $mode);
				}
			}
		}
		else if (is_string($this->groupBy))
		{
			$options = regroupList($this->options, $this->groupBy);
		
			foreach($options as $group => $choices)
			{
				echo "<br/><h4>$group</h4>";
				$this->renderCheckboxes($field, $choices, $itempk, $selected, $mode);
			}
		}
		else
		{
			$this->renderCheckBoxes($field, $this->options, $itempk, $selected, $mode);
		}
	
		echo "</div>\n";
	
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
		
			
		$this->_startField($field);
	
		if (is_array($this->groupBy) && count($this->groupBy) > 0)
		{
			$gpk = $this->groupBy[0]->getPrimaryKey();
			
			foreach($this->groupBy as $group)
			{
				if (array_key_exists($group->get($gpk), $this->options))
				{
					echo "<span class='cross_reference_group'>".$group->format($this->groupByFormat)."</span>";
					foreach($this->options[$group->get($gpk)] as $item)
					{
						if (array_key_exists($item->$optpk, $items))
						{
							echo $this->formatName($item, $this->nameField)."<br/>";
						}
					}
				}
			}
		}
		else
		{					
			$optpk = $this->getMatchingPK($xref, $this->options[0]);
			
			$items = indexedQuery($this->xrefClass, "WHERE {$pk}=".$obj->get($pk), $optpk);
			
			foreach($this->options as $item)
			{
				if (array_key_exists($item->$optpk, $items))
				{
					echo $this->formatName($item, $this->nameField)."<br/>";
				}
			}
		}
		
		$this->_endField($field);
	}
	
	function postProcess($field = "")
	{
		if (count($this->options) == 0) return;

		$xref = new $this->xrefClass;
		
		$obj = $this->parent->data;
		$pk =  $this->getMatchingPK($xref, $obj);
		
		if (is_array($this->groupBy) && count($this->groupBy) > 0)
		{
			$keys = array_keys($this->options);
			$rc = $this->options[$keys[0]][0];
		}
		else
		{
			$rc = $this->options[0];
		}
		
		$optpk = $this->xrefField ? $this->xrefField : $this->getMatchingPK($xref, $rc);
		
		$this->clearOutRelations($obj, $pk);
		
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
	
	/**
	 * Clear out the current relations stored in the xref class.
	 * 
	 * @param Object $obj
	 * @param String $pk
	 */
	function clearOutRelations($obj, $pk)
	{
		$xref = new $this->xrefClass;
		$xref->delete("WHERE {$pk}=".$obj->get($pk));
	}		
}


/**@}*/?>