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
 * Field renderer for zipcode data fields.
 * 
 * autocomplete: whether field may attempt autocompletion based on the
 * user-entered data string. 
 * 
 * onChange: script-defined callback function to call when user edits
 * the field (script is called on field exit, not for each key entered)
 * 
 * disable: alternative to readonly; field can be reenabled thru javascript
 * 
 * Example:
 * to disable input in a field, add the following line of code to your
 * calling script:
 * $form->getRenderer("foo")->disable = true;
 * 
 */
class ZipCodeFieldRenderer extends FieldRenderer
{	
	var $autocomplete = true; // default
	var $onChange = ""; // optional callback on data input change
	var $disable = false; // alternative to readonly; field can be reenabled thru javascript
	var $includePlus4 = true;
	var $international = false;
	var $internationalLabel = "Postal Code";
	
	function ZipCodeFieldRenderer(&$form)
	{	
		$this->FieldRenderer($form);
	}

	function setInternational($label = "Postal Code")
	{
		$this->international = true;
	}
	
	function addValidatorsToForm($field, $required = false)
	{
		$label = isset($this->label) ? $this->label : $this->parent->prettifyFieldName($field);
		
		if($required)
		{
			if (!$this->international)
			{
				$expr = $this->includePlus4 ? "^\\d{5}(?:-\\d{4})?$" : "^\\d{5}$";
				
				$this->parent->regexp($field, $expr, "Please supply a valid zip code.");
			}
			
			$this->parent->validator->add(new RequiredValidator($field, $label));
		}	
	}
		
	function renderScript($field)
	{
		$size = $this->includePlus4 ? 10 : 5;
?>
<script type='text/javascript'>
/* <![CDATA[ */
	function <?echo $field ?>_keyup(event)
	{
		var ctrl = document.id('<? echo "{$this->parent->id}_$field" ?>');
		var len = ctrl.value.length;
		if (len >= <?echo $size?>) 
		{
			ctrl.value = ctrl.value.substring(0, <?echo $size?>);
			len = <?echo $size?>;
		}
		
		var count = ctrl.length;

		// JDG 2/1/10 must check count to fix IE7 bug
		if(count)
			count.innerHTML = len;
	}
</script>
<?
	}
	
	function renderField($field)
	{
		$this->_startField($field);

		
		if (!$this->international)
		{	
			$size = $this->includePlus4 ? 10 : 5;
			$onkeyup = " onkeyup='{$field}_keyup(event);'";
			$onkeypress=" onkeypress='return maskInput(event)'";
		}
		else
		{
			$size = 12;
		}
		
		$autocomplete = (!$this->autocomplete) ? "autocomplete='off' " : "";	
		$onchange = ($this->onChange) ? "onchange='$this->onChange(this)'" : "";
		$disable = ($this->disable) ? "disabled='disabled'" : "";
		
		echo "<input id='{$this->parent->id}_{$field}' 
		type='text' style='width: auto' name='$field' $onchange $autocomplete 
		value='".htmlspecialchars($this->parent->data->get($field), ENT_QUOTES, 'UTF-8')."' 
		size='{$size}' $disable $onkeypress $onkeyup/>";
		$this->_endField($field);
	}
	
	
	function renderReadOnly($field)
	{
		$this->_startField($field);

		echo $this->parent->data->get($field);
		
		$this->_endField($field);
	}

	function renderSearchField($field, $mode = "equal")
	{
		$this->_startField($field);
		
		$onkeyup = " onkeyup='{$field}_keyup(event);'";
		$onkeypress=" onkeypress='return maskInput(event)'";
		
		if ($mode == "range")
		{
			$from = htmlspecialchars($this->parent->params->get($field, "from"), ENT_QUOTES, 'UTF-8');
			$to =  htmlspecialchars($this->parent->params->get($field, "to"), ENT_QUOTES, 'UTF-8');
			
			echo "From <input type='text' id='$field' name='$field:from' value='{$from}' size='10'$onkeypress$onkeyup> to <input type='text' name='$field:to' value='{$to}' size='10'$onkeypress$onkeyup>";
		}
		else
		{
			$value =  htmlspecialchars($this->parent->params->get($field, $mode), ENT_QUOTES, 'UTF-8');
			echo "<input type='text' id='$field' name='$field:$mode' value='{$value}' size='10'$onkeypress$onkeyup>";
		}

		$this->_endField($field);
		
	}
	
}

?>