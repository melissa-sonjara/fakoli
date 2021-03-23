<?php
/**\addtogroup fieldrenderers
 * @{
 */

/**************************************************************

Copyright (c) 2007-2018 Sonjara, Inc

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
 * Field renderer for boolean data fields. Renders as a toggle.
 */
class ToggleFieldRenderer extends FieldRenderer
{
	var $disable = false; ///, alternative to readonly; field can be reenabled thru javascript
	var $template = "Yes/No"; ///< How to format read only
	var $onClick = null; ///< Name of Javascript function to be called when checkbox is toggled
	var $style = "round";
	var $ragged = false;
	
	/**
	 * Create a new BooleanFieldRenderer
	 * @param AutoForm $parent the parent form
	 */
	function ToggleFieldRenderer(&$parent, $ragged, $field = null, $label = null)
	{
		$this->FieldRenderer($parent);
		
		$this->ragged = $ragged;
		if ($this->ragged)
		{
			$this->hideLabel = true;
			$this->colspan = 2;
		}
		
		if ($field)
		{
			if (!$parent->data->hasField($field))
			{
				$parent->add($this, $field, $label);
			}
			else
			{
				$parent->override($field, $label, $this);
			}
		}
	}
	
	function addValidatorsToForm($field, $required = false)
	{
		$label = isset($this->label) ? $this->label : $this->parent->prettifyFieldName($field);
		
		if($required)
		{
			$this->parent->validator->add(new RequiredBooleanValidator($field, $label));
		}
		
	}
	
	
	function renderField($field)
	{

		
		$this->_startField($field);
		$disable = ($this->disable) ? " disabled" : "";
		$onClick = ($this->onClick) ? " onclick='return {$this->onClick}(this.checked);'" : "";
		
		if ($this->ragged)
		{
			echo "<label class='label' for='{$this->parent->id}_{$field}'>".$this->_getLabel($field, false)."</label>\n";
		}
		
		echo "<label class='switch{$disable}'>";
		echo "<input id='{$this->parent->id}_{$field}' type='checkbox' class='checkbox' value='1' name='$field'";
		if ($this->parent->data->get($field))
		{
			echo " checked";
		}
		echo " {$disable}{$onClick}>&nbsp;<span class='slider {$this->style}'></span>";
		echo "</label>";
		
		$this->_endField($field);
	}
	
	function renderSearchField($field, $mode = "match")
	{
		echo "<tr>\n";
		echo "<td colspan='2'";
		if ($this->parent->labelCSS) echo " class='{$this->parent->labelCSS}'";
		echo "><input type='checkbox' value='1' name='$field:$mode'";
		if ($this->parent->params->get($field, $mode))
		{
			echo " checked";
		}
		echo ">&nbsp;";
		echo $this->parent->prettifyFieldName($field);
		echo "</td>\n</tr>\n";
	}
	
	function renderReadOnly($field)
	{
		$this->colspan = 1;
		$this->hideLabel = false;
		
		$this->_startField($field);
		echo $this->format($field);
		$this->_endField($field);
	}
	
	function format($field)
	{
		return BooleanTypeRenderer::format($this->parent->data->get($field), $this->template);
	}
}

/**@}*/