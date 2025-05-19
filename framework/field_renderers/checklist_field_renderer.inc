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
 * CheckListFieldRenderer: Renders the specified list of options as a list of checkboxes
 * from which multiple values can be selected. The results are stored in the associated field
 * as a comma-separated list of values.
 * 
 * @param obj $form the AutoForm object, passed by reference
 * 
 * @param string $field the name of the field defined in the dataitem class object 
 * 		   in the datamodel folder whose values can be changed by the checkbox list 
 * 		   This field name should match the field name in the database.
 * 
 * @param string $label presented on the form to the user to describe the list
 * 
 * @param array $options defines the list of options available, as follows:
 * 			  $options = array (
 * 					1	=>	"Option 1",
 *					2	=>	"Option 2",
 *					3	=>	"Option 3"
 * 					)
 * 			For grouped lists, the structure would be:
 * 
 * 				$options = 
 * 					array("Group 1" =>
 * 						array(
 * 							1 => "Option 1",
 * 							2 => "Option 2"),
 * 						  "Group 2"	=>
 * 						array(
 * 							3 => "Option 3"));
 * 
 * @param boolean $grouped specifies whether the array defines a grouping structure
 * 			for the checkbox list. The default is false. 
 */
class CheckListFieldRenderer extends FieldRenderer
{
	var $options;
	var $addEntry;
	var $addEntryText;
	var $grouped;
	var $addEntrySize= 20;
	var $width;
	var $height;
	var $resizable = false;
	var $nobr = false;
	var $dropdown;
	var $dropdownMaxWidth = "340px";
	var $dropdownMessage = "Click to Select Terms";
	var $showSelectAll = false;
	var $separator = ", "; 				///< The separator between values in read only list
	
	var $fixedOptions = array();
	var $defaultOptions = array();
	
	function __construct(&$form, $field, $label, $options, $grouped = false)
	{
		$this->options = $options;
		$this->grouped = $grouped;
		parent::__construct($form);
		if ($form->data->hasField($field))
		{
			$form->override($field, $label, $this);
		}
		else
		{
			$form->add($this, $field);
			if ($label) $form->alias($field, $label);
		}
	}
			
	function allowAddEntry($text = "or add new", $size = 40)
	{
		$this->addEntry = true;
		$this->addEntryText = $text;
		$this->addEntrySize = $size;
		
		return $this;
	}
	
	function setSize($width, $height, $scrollAfter = 0)
	{
		$this->width = $width;
		$this->height = $height;
		$this->scrollAfter = $scrollAfter;
		
		return $this;
	}

	function addFixedOption($value)
	{
		$this->fixedOptions[$value] = $value;
		
		return $this;
	}
	
	function addDefaultOption($value)
	{	
		$this->defaultOptions[$value] = $value;
		
		return $this;
	}
	
	function getOptionCount()
	{
		if (!$this->grouped) return count($this->options);
		
		$count = 0;
		
		foreach($this->options as $groupName => $group);
		{
			$count += count($group) + 1;
		}
		
		return $count;
	}
	
	function renderField($field)
	{
		$this->_startField($field);
		
		$rawValue = $this->parent->data->get($field);
		if (is_array($rawValue))
		{
			$this->preProcess($field);
			$rawValue = $this->parent->data->get($field);
		}
		
		$values = explode(",", $rawValue);		
				
		$divider = $this->horizontal ? " " : "";
		$layout = $this->horizontal ? " style='display: inline'" : "";
		
		if ($this->showSelectAll)
		{
			echo "<a class='select_toggle' href='#' onclick='return {$this->parent->id}_{$field}_toggleSelectAll(this);'>Select All</a><br/>";
		}
		
		if ($this->width && $this->height && $this->getOptionCount() > $this->scrollAfter)
		{
			$width = is_numeric($this->width) ? "{$this->width}px" : $this->width;
			$height = is_numeric($this->height) ? "{$this->height}px" : $this->height;
			$padding = ($width == "auto") ? "padding-right: 20px; " : "";
			
			$resizable = ($this->resizable && !$this->dropdown) ? " resizable": "";
			$dropdown = $this->dropdown ? " multi_select_dropdown_list" : "";
			echo "<div id='{$this->parent->id}_{$field}_container' class='selectbox scrollbox{$resizable}{$dropdown}' style='width: {$width}; height: {$height}; $padding overflow: auto'>\n";
		}
		else
		{
			echo "<div class='selectbox'>";
		}
		
		$idx = 0;
		
		if ($this->grouped)
		{
			echo "<dl>\n";
			foreach($this->options as $groupName => $group)
			{
				echo "<dt>$groupName</dt>\n<dd>\n";
				foreach($group as $value => $text)
				{
					$checked = (array_search($value, $values) !== FALSE) ? " checked" : "";
					
					if ($this->nobr) { $text = str_replace(" ", "&nbsp;", $text); }
					
					echo "<label for='{$this->parent->id}_{$field}[$idx]'$layout><input type='checkbox' class='checkbox' id='{$this->parent->id}_{$field}[$idx]' name='{$field}[$idx]' value='$value'$checked/>$text</label>$divider";
					$idx++;
				}
				
				echo "</dd>";
			}
			echo "</dl>";
		}
		else
		{
			if(count($this->options) > 0)
			{
				foreach($this->options as $value => $text)
				{
					$disabled = "";
					
					if ($this->fixedOptions[$value])
					{
						$disabled = " disabled";
						$checked = " checked";
					}
					else if ($this->defaultOptions[$value])
					{
						$checked = "checked";
					}
					else
					{
						$checked = (array_search($value, $values) !== FALSE) ? " checked" : "";
					}
					if ($this->nobr) { $text = str_replace(" ", "&nbsp;", $text); }
					echo "<label for='{$this->parent->id}_{$field}[$idx]'$layout><input type='checkbox' class='checkbox' id='{$this->parent->id}_{$field}[$idx]' name='{$field}[$idx]' value='$value'$checked$disabled/>$text</label>$divider";
					$idx++;
				}

				if($this->addEntry)
				{
					foreach($values as $value)
					{
						if(!array_key_exists($value, $this->options))
							$addEntryValue .= $value;
					}
				}
			}
		}
		
		if ($this->addEntry)
		{
			echo "&nbsp;{$this->addEntryText}&nbsp;";
		}
		
		if ($this->addEntry)
		{
			echo "<input type='text' name='{$field}_addEntry' value='{$addEntryValue}' size='{$this->addEntrySize}'/>";
		}
		
		// Save this count to loop through checkboxes on required validation
		echo "<input type=\"hidden\" name=\"count_{$field}\" value=\"".$idx."\">";		
		
		echo "</div>\n";
		
		$this->_endField($field);
	}
		
	function renderScript($field)
	{
		if ($this->showSelectAll)
		{
			?>
	<script type="text/javascript">
	
	function <?echo $this->parent->id?>_<?echo $field ?>_toggleSelectAll(link)
	{
		link = document.id(link);
		
		var value = (link.innerHTML == "Select All");
	
		var container = document.id('<?echo $this->parent->id?>_<?echo $field?>_container');
		
		container.getElements("input[type=checkbox]").each(function(cbox)
		{
			cbox.set('checked', value);
		});
	
		link.innerHTML = (value) ? "Deselect All" : "Select All";
		if (container.multiSelect) container.multiSelect.selectionChanged();
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

	function renderReadOnly($field)
	{
		$this->_startField($field);
		
		$value = $this->parent->data->get($field);

		$items = explode(",", $value);
		
		if ($this->grouped)
		{
			foreach($this->options as $category => $options)
			{
				$out = "";
			
				foreach($items as $item)
				{
					if (array_key_exists($item, $options))
					{
						if (!$out) 
						{
							$out = "$category: ";
						}
						else
						{
							$out .= $this->separator;
						}
						
						$out .= $options[$item];
					}
				}
				
				if ($out) $answers[] = $out;
			}
		}
		else
		{
			// JDG 3/12 - show option value, if found, instead of key
		
			foreach($items as $item)
			{
				$answers[] = array_key_exists($item, $this->options) ? $this->options[$item] : $item;
			}
		}
		
		$value = implode($this->separator, $answers);
		
		echo $value;

		$this->_endField($field);
	}

	function renderSearchScript($field, $mode)
	{
		$this->renderScript($field);
	}
	
	/*
	 * Only implemented for ungrouped option set
	 */
	function renderSearchField($field, $mode)
	{
		$this->_startField($field);
		
		$rawValue = $this->parent->params->get($field, $mode);
		
		if (is_array($rawValue))
		{
			$rawValue = implode(",", array_values($rawValue));
			$this->parent->params->set($field, $mode, $rawValue);
		}
		
		$values = explode(",", $rawValue);		
		$values = array_combine($values, $values);

		$divider = $this->horizontal ? " " : "";
		$layout = $this->horizontal ? " style='display: inline'" : "";
		
		if ($this->showSelectAll)
		{
			echo "<a href='#' onclick='return {$this->parent->id}_{$field}_toggleSelectAll(this);'>Select All</a><br/>";
		}
		
		if ($this->width && $this->height && $this->getOptionCount() > $this->scrollAfter)
		{
			$width = is_numeric($this->width) ? "{$this->width}px" : $this->width;
			$height = is_numeric($this->height) ? "{$this->height}px" : $this->height;
			$padding = ($width == "auto") ? "padding-right: 20px; " : "";
			
			$resizable = ($this->resizable && !$this->dropdown) ? " resizable": "";
			$dropdown = $this->dropdown ? " multi_select_dropdown_list" : "";
			echo "<div id='{$this->parent->id}_{$field}_container' class='selectbox scrollbox{$resizable}{$dropdown}' style='width: {$width}; height: {$height}; $padding overflow: auto'>\n";
		}
		else
		{
			echo "<div class='selectbox'>";
		}		
		$idx = 0;

		
		if ($this->grouped)
		{
			echo "<dl>\n";
			foreach($this->options as $groupName => $group)
			{
				echo "<dt>$groupName</dt>\n<dd>\n";
				foreach($group as $value => $text)
				{
					$checked = (array_search($value, $values) !== FALSE) ? " checked" : "";
					
					if ($this->nobr) { $text = str_replace(" ", "&nbsp;", $text); }
					
					echo "<label for='{$this->parent->id}_{$field}_$idx'$layout><input type='checkbox' class='checkbox' id='{$this->parent->id}_{$field}_$idx' name='{$field}:{$mode}[$idx]' value='$value'$checked/>$text</label>$divider";
						
					$idx++;
				}
				
				echo "</dd>";
			}
			echo "</dl>";
		}
		else
		{
			if(count($this->options) > 0)
			{
				foreach($this->options as $value => $text)
				{
					$checked = (array_key_exists($value, $values)) ? " checked" : "";
					echo "<label for='{$this->parent->id}_{$field}_$value'$layout><input type='checkbox' class='checkbox' id='{$this->parent->id}_{$field}_$value' name='{$field}:{$mode}[$value]' value='$value'$checked/>$text</label>$divider";
						
					$idx++;
				}
			}
		}
				
		if ($this->width && $this->height)
		{
			echo "</div>\n";
		}
		
		$this->_endField($field);
	}
	
	function preProcess($field = "")
	{		
		$value = ($this->parent->method == "POST") ? $_POST[$field] : $_GET[$field];
		$addEntry = ($this->parent->method == "POST") ? $_POST["{$field}_addEntry"] : $_GET["{$field}_addEntry"];

		if (is_array($value))
		{
			$this->parent->data->set($field, implode(",",  array_values($value)));
		}
		else if (!$value)
		{
			$this->parent->data->set($field, "");
		}
		else
		{
			$this->parent->data->set($field, $value);
		}
		
		trace("addEntry($field): $addEntry", 3);
		
		if ($addEntry)
		{
			if($this->parent->data->get($field))
			{
				$this->parent->data->set($field, $this->parent->data->get($field) . ",");
			}
			
			$this->parent->data->set($field, $this->parent->data->get($field) . $addEntry);
		}
	}
	
}
/**@}*/?>