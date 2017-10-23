<?php
/**************************************************************

Copyright (c) 2015 Sonjara, Inc

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

class SliderFieldRenderer extends FieldRenderer
{
	var $min = 0;
	var $max = 10;
	var $editable = true;
	var $thresholds = null;
	var $displayValue = true;
	
	function __construct(&$form, $field, $min = 0, $max = 10, $editable = true)
	{
		$this->min = $min;
		$this->max = $max;
		$this->editable = $editable;
		$this->FieldRenderer($form);
		
		if ($field && !$form->data->hasField($field))
		{
			$form->add($this, $field);
		}
		else
		{
			$form->override($field, $label, $this);
		}
	}
	
	function setThresholds($thresholds)
	{
		$this->thresholds = $thresholds;
	}
	
	function renderScript($field)
	{
		if (isset($this->thresholds))
		{
			krsort($this->thresholds, SORT_NUMERIC);
			$thresholds = json_encode($this->thresholds);
		}
		else
		{
			$thresholds = "null";
		}
		
		$id = "{$this->parent->id}_{$field}";
			
		$script = <<<ENDSCRIPT
<script type='text/javascript'>
var {$id}_thresholds = {$thresholds};
window.addEvent('domready', function()
{
	var range = document.id('{$id}');
	var display = document.id('{$id}_display');

	if (display != null)
	{
		range.addEvent('input', function(elt)
		{
			if ({$id}_thresholds != null)
			{
				for(var score in {$id}_thresholds)
				{
					if (range.value == score)
					{
						display.set('html', ${id}_thresholds[score]);
					}
				}
			}
			else
			{
				display.set('text', range.value);
			}
		});
	}
});
</script>
ENDSCRIPT;
		
		echo $script;
	}
		
	function renderField($field)
	{
		$this->_startField($field);
		$value = $this->parent->data->get($field);
		if (!$value) $value = $this->min;
		
		echo "<input type='range' id='{$this->parent->id}_{$field}' name='{$field}' min='{$this->min}' max='{$this->max}' value='{$value}' data-displayField='{$this->parent->id}_{$field}_display'/>";
		
		if ($this->displayValue)
		{
			$threshold = ($this->thresholds) ? $this->thresholds[$value] : "";
			echo "<span class='range_display' id='{$this->parent->id}_{$field}_display'>{$threshold}</span>";
		}
		
		$this->_endField($field);
	}
	
	function renderReadOnly($field)
	{
	}
}