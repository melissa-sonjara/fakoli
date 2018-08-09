<?php

/**************************************************************

 Copyright (c) 2018 Sonjara, Inc

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


class SubSelectCheckListFieldRenderer extends CheckListFieldRenderer
{
	var $super;
	var $format = null;
	
	function __construct(&$form, $field, $label, $super, $options, $format = null)
	{
		$this->options = $options;
		$this->super = $super;
		$this->format = $format;
		
		$this->CheckListFieldRenderer($form, $field, $label, $options, true);
	}
	
	function renderScript($field = null)
	{
		Fakoli::includeScript("/fakoli/js/subselect_checklist_manager.js");
		
	}
	
	function renderField($field = "")
	{
		$this->_startField($field);
		
		echo "<pre>".print_r($this->options, true)."</pre>";
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

		foreach($this->options as $group => $options)
		{
			foreach($options as $value => $item)
			{
				$disabled = "";
				
				$text = ($this->format) ? $item->format($this->format) : $item;
				
				if ($this->fixedOptions[$value])
				{
					$disabled = " disabled";
					$checked = " checked";
				}
				else
				{
					$checked = (array_search($value, $values) !== FALSE) ? " checked" : "";
				}
				if ($this->nobr) { $text = str_replace(" ", "&nbsp;", $text); }

				echo "<label data-select='{$group}' for='{$this->parent->id}_{$field}[$idx]'$layout><input type='checkbox' class='checkbox' id='{$this->parent->id}_{$field}[$idx]' name='{$field}[$idx]' value='$value'$checked$disabled/>$text</label>$divider";
				$idx++;
			}
		}
				
		// Save this count to loop through checkboxes on required validation
		echo "<input type=\"hidden\" name=\"count_{$field}\" value=\"".$idx."\">";		
		
		echo "</div>\n";
		
		$this->_endField($field);
	}
}