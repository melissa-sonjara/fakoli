<?php

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