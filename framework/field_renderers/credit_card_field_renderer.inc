<?php

/**************************************************************

Copyright (c) 2014 Sonjara, Inc

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

class CreditCardNumberFieldRenderer extends FieldRenderer
{
	var $size = 19;
	var $onKeyUp; // javascript callback function
	var $disable = false; // alternative to readonly; field can be reenabled or set thru javascript
	var $onChange = ""; // optional callback on data input change
	
	function CreditCardNumberFieldRenderer(&$parent, $field = null)
	{
		$this->FieldRenderer($parent);
		if ($field && !$parent->data->hasField($field))
		{
			$parent->add($this, $field);
		}
	}
	
	function addValidatorsToForm($field, $required = false)
	{
		$label = isset($this->label) ? $this->label : $this->parent->prettifyFieldName($field);
		
		$this->parent->validator->add(new CreditCardNumberValidator($field, $label));
		
		if($required)
		{
			$this->parent->validator->add(new RequiredValidator($field, $label));
		}
		
	}

	function renderScript($field)
	{
		$id = "{$this->parent->id}_{$field}";
?>
<script type='text/javascript'>
function <?echo $id?>_checkValid(input)
{
	if (luhnCheck(input.value) && input.value.length > 12 && input.value.length < 20)
	{
		input.addClass('valid');
	}
	else
	{
		input.removeClass('valid');
	}
}	
</script>
<?
	}
			
	function renderField($field)
	{
		$this->_startField($field);
	
		$disable = ($this->disable) ? "disabled='disabled'" : "";

		$id = "{$this->parent->id}_{$field}";
		
		$onchange = ($this->onChange) ? "onchange='{$this->onChange}(this)'" : "";
	
		echo "<input id='{$id}' type='text' class='credit_card_number' style='width: auto' name='$field'
			value='".$this->parent->data->get($field)."' autocomplete='off'
			size='{$this->size}' onkeypress='return maskInput(event, 0);' $disable $onchange onkeyup='{$id}_checkValid(this)'/>";
	
		$this->_endField($field);
	}
	
	function renderReadOnly($field)
	{
		$this->_startField($field);
		
		echo $this->parent->data->get($field);
		
		$this->_endField($field);
	}	
}

// Register the field renderer
AutoForm::registerFieldRendererClass(CreditCardNumber, CreditCardNumberFieldRenderer);