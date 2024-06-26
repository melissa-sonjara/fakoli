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

/*
 * Render a string phone number into separate
 * boxes for area code then 3 digits, 4, and a space for
 * extension, if applicable.
 * 
 */
 

class PhoneNumberFieldRenderer extends FieldRenderer
{	
	var $disable = false; // alternative to readonly; field can be reenabled thru javascript
	var $template = "standard"; // PhoneNumberTypeRenderer template for reaonly format (e.g., "(123) 456-7890 ext 123")
	var $extension = false;
		
	function PhoneNumberFieldRenderer(&$parent)
	{
		$this->FieldRenderer($parent);
	}
	
	function renderScript($field)
	{
		if ($this->parent->readOnlyForm || $this->parent->isReadOnly($field)
			|| array_key_exists($field, $this->parent->hidden)) return "";

		$parts = PhoneNumberTypeRenderer::$parts;
?>
<script type='text/javascript'>
<?php		
	foreach($parts as $part => $limit)
	{
		if($part == "extension" && !$this->extension) 
			continue;
		$partField = ($part) ? "{$field}_{$part}" : $field;
		$this->renderScriptOnePart($partField, $limit);
	}
?></script><?php
	}
	
	function renderScriptOnePart($field, $limit)
	{
?>
function <?echo $this->parent->id?>_<?echo $field ?>_keyup(event)
{
	var ctrl = document.id('<?echo "{$this->parent->id}_{$field}" ?>');
	if(!ctrl) return;
		
	if (event.key == "Tab" || event.key == "Shift") return;
	
	if(ctrl.value.length == <?php echo $limit ?>)
	{
		var next = ctrl.getNext("input");
		if(next && next.getAttribute('data-name') == ctrl.getAttribute('data-name'))
		{
			next.focus();
		}
	}
	else if (ctrl.value.length >= <?echo $limit ?>) 
	{
		ctrl.value = ctrl.value.substring(0, <?echo $limit ?>);
	}
	
}
<?php
	}
	
	function renderField($field)
	{
		$parts = PhoneNumberTypeRenderer::$parts;
		
		$this->_startField($field);

		$value = $this->parent->data->get($field);

		$valueParts = PhoneNumberTypeRenderer::getValueParts($value);
		
		$idx = 0;
		foreach($parts as $part => $limit)
		{
			if($part == "extension" && !$this->extension)
				continue;
			
			if($part == "area_code")
				$pre = "(&nbsp;";
			elseif($part == "exchange")
				$pre = ")&nbsp;&nbsp;";
			elseif($part == "extension")
				$pre = "ext ";
			else
				$pre = "<b>&#65293;</b>";
				
			$partField = ($part) ? "{$field}_{$part}" : $field;
			
			$onkeypress = " onkeyup='{$this->parent->id}_{$partField}_keyup(event);'";	
			$disable = ($this->disable) ? "disabled='disabled'" : "";
			echo "$pre<input id='{$this->parent->id}_{$partField}' style='width: auto' data-name='{$field}' type='text' name='{$partField}' value='".htmlspecialchars($valueParts[$part], ENT_QUOTES, 'UTF-8')."' size='$limit' onkeypress='return maskInput(event, false);' $disable $onkeypress/>&nbsp;";
			$idx++;	
		}
		
		$this->_endField($field);
	}
	
	
	function renderReadOnly($field)
	{
		$this->_startField($field);

		echo $this->format($field);

		$this->_endField($field);
	}

	function renderSearchField($field, $mode = "equal")
	{
		if ($mode == "range")
		{
			$from = htmlspecialchars($this->parent->params->get($field, "from"), ENT_QUOTES, 'UTF-8');
			$to =  htmlspecialchars($this->parent->params->get($field, "to"), ENT_QUOTES, 'UTF-8');
			
			echo "<tr>\n";
			$this->_printLabel($field);
			echo "<td>From <input type='text' id='$field' name='$field:from' value='{$from}' size='25'> to <input type='text' name='$field:to' value='{$to}' size='25'></td>\n";		
			echo "</tr>\n";
		}
		else
		{
			$value =  htmlspecialchars($this->parent->params->get($field, $mode), ENT_QUOTES, 'UTF-8');
			echo "<tr>\n";
			$this->_printLabel($field);
			echo "<td><input type='text' id='$field' name='$field:$mode' value='{$value}' size='50'></td>\n";		
			echo "</tr>\n";
		}		
	}
	
	function preProcess($field = "")
	{		
		$parts = PhoneNumberTypeRenderer::$parts;
		
		$partValues = array();
		
		foreach($parts as $part => $limit)
		{
			$partField = ($part) ? "{$field}_{$part}" : $field;
			$partValues[] = $_POST[$partField];
		}

		$value = implode($partValues, "");
		$this->parent->data->set($field, $value);
	}
		
		
	/*
	 * Instead of a custom validator, use regexp to 
	 * validate each individual part of the phone number.
	 * The extension needs no validation because it can
	 * be blank and is already guaranteed to be only digits
	 * by the keyup event on the input element.
	 */
	function addValidatorsToForm($field, $required = false)
	{
		$label = isset($this->label) ? $this->label : $this->parent->prettifyFieldName($field);

		$parts = PhoneNumberTypeRenderer::$parts;
		
		$message = "Please supply phone as a 10-digit number.";
		foreach($parts as $part => $limit)
		{
			$partField = ($part) ? "{$field}_{$part}" : $field;
			if($part == "extension") continue;
			$regexp = "^\\d{" . $limit . "}$";
			$this->parent->validator->add(new RegularExpressionValidator($partField, $label, $regexp, $message));
		}
			
		if($required)
			$this->parent->validator->add(new RequiredValidator($field, $label));
	}
		

	function format($field)
	{
		return PhoneNumberTypeRenderer::format($this->parent->data->get($field), $this->template);
	}
		
}

/**@}*/