<?php

/**************************************************************

Copyright (c) 2020 Sonjara, Inc

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
 * Field renderer for boolean data fields. Renders as a checkbox.
 */
class ColorFieldRenderer extends FieldRenderer
{
	var $disable = false; ///, alternative to readonly; field can be reenabled thru javascript
	var $onChange = null; ///< Name of Javascript function to be called when checkbox is toggled
	
	/**
	 * Create a new BooleanFieldRenderer
	 * @param AutoForm $parent the parent form
	 */
	function __construct(&$parent, $field = null, $label = null)
	{
		$this->FieldRenderer($parent);
		if ($field && !$parent->data->hasField($field))
		{
			$parent->add($this, $field, $label);
		}
	}	
	
	function renderScript($field)
	{
		$f = "{$this->parent->id}_{$field}";
		$onChange = ($this->onChange) ? " {$this->onChange}(elt);" : ";";
		
		echo <<<ENDSCRIPT
	<script type='text/javascript'>
	function {$f}_onChange(elt)
	{
		document.id('{$f}_color').value = elt.value;
		return{$onChange}
	}
		
	function {$f}_color_onChange(elt)
	{
		document.id('{$f}').value = elt.value;
		return{$onChange}
	}
	</script>
ENDSCRIPT;
	}
	
	function renderField($field)
	{
		$this->_startField($field);
		
		$val = $this->parent->data->get($field);
		
		$f = "{$this->parent->id}_{$field}";
		$disable = ($this->disable) ? "disabled='disabled'" : "";
		
		echo "<input id='{$f}' class='color' type='text' size='7' value='{$val}' name='$field' {$disable} onkeypress='return {$f}_onChange(this);'>&nbsp;";
		echo "<input id='{$f}_color' type='color' value='{$val}' name='{$field}' {$disable} onchange='return {$f}_color_onChange(this);'>";
		
		$this->_endField($field);
	}
	
	function renderSearchField($field, $mode = "match")
	{
		$this->_startField($field);
		
		$val = $this->parent->data->get($field);
		
		$disable = ($this->disable) ? "disabled='disabled'" : "";
		$onChange = ($this->onChange) ? " onchange='return {$this->onChange}(this);'" : "";
		echo "<input id='{$this->parent->id}_{$field}' type='color' value='{$val}' name='$field:$mode' {$disable}{$onChange}>";
		
		$this->_endField($field);
	}
	
	function renderReadOnly($field)
	{
		$this->_startField($field);
		
		$val = $this->parent->data->get($field);
		
		echo "<input id='{$this->parent->id}_{$field}' type='color' value='{$val}' name='$field' readonly='readonly'>";
		
		$this->_endField($field);
	}
}

?>