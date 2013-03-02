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
 * Field renderer for string data fields. Renders as a single-line text field.
 * 
 * size: the length of the input field
 * 
 * limit: maximum number of characters user may enter
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
class StringFieldRenderer extends FieldRenderer
{	
	var $size = 50;
	var $limit = 0; 			///< optional - restricts users from entering character count over limit
	var $autocomplete = true; 	///< default is true. Set to false to disable autocompletion
	var $onChange = ""; 		///< optional (javascript) callback on data input change
	var $disable = false; 		///< alternative to readonly; field can be reenabled thru javascript
	var $password = false;		///< Render the field as a password field (masked characters)
	var $template = ""; 		///< StringTypeRenderer template for readonly format (e.g., "prettify")
	var $confirm = false;		///< Render a second field for the user to confirm their entry (for email addresses, etc)
	var $width = "";				///< Optionally specify a CSS width for the field
	
	function StringFieldRenderer(&$parent)
	{
		$this->FieldRenderer($parent);
	}
	
	function renderScript($field)
	{
		if ($this->limit)
		{
?>
<script type='text/javascript'>
/* <![CDATA[ */
	function <?echo $field ?>_keyup(event)
	{
		var form = $('<?echo $this->parent->id ?>');
		
		var ctrl = form['<?echo $field ?>'];
		var len = ctrl.value.length;
		if (len >= <?echo $this->limit?>) 
		{
			ctrl.value = ctrl.value.substring(0, <?echo $this->limit ?>);
			len = <?echo $this->limit ?>;
		}
		
		var count = document.getElementById('<?echo $field ?>_length');

		// JDG 2/1/10 must check count to fix IE7 bug
		if(count)
			count.innerHTML = len;
	}
</script>
<?
		}
	}
	
	function renderField($field)
	{
		$this->_startField($field);
		
		if ($this->limit)
		{
			echo "<div>";
			$onkeypress = " onkeyup='{$field}_keyup(event);'";
		}
		
		$autocomplete = (!$this->autocomplete) ? "autocomplete='off' " : "";		
		$onchange = ($this->onChange) ? "onchange='$this->onChange(this)'" : "";		
		$disable = ($this->disable) ? "disabled='disabled'" : "";		
		$inputType = (!$this->password) ? "text" : "password";
		$style = ($this->width) ? " style='width: {$this->width}'" : "";
		
		echo "<input id='{$this->parent->id}_{$field}' type='$inputType' name='$field' $onchange $autocomplete $style value='".htmlspecialchars($this->parent->data->get($field), ENT_QUOTES, 'UTF-8')."' size='{$this->size}' $disable $onkeypress/>";

		if ($this->limit)
		{
			echo "</div>";
		}
		$this->_endField($field);
		
		if ($this->confirm)
		{
			$confirm = "confirm_$field";
			$this->_startField($confirm);
			
			$autocomplete = (!$this->autocomplete) ? "autocomplete='off' " : "";		
			$disable = ($this->disable) ? "disabled='disabled'" : "";		
			$inputType = (!$this->password) ? "text" : "password";
			echo "<input id='{$this->parent->id}_{$confirm}' type='$inputType' name='$confirm' $autocomplete value='".htmlspecialchars($this->parent->data->get($field), ENT_QUOTES, 'UTF-8')."' size='{$this->size}' $disable/>";
		
			$this->_endField($field);
		}	
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
	
		
	function addValidatorsToForm($field, $required = false)
	{
		$label = isset($this->label) ? $this->label : $this->parent->prettifyFieldName($field);
		$confirm = "confirm_$field";
		$confirmLabel = "Confirm $label";
		
		if($required)
			$this->parent->validator->add(new RequiredValidator($field, $label));
			
		if ($this->confirm)
		{
			$this->parent->validator->add(new MatchValidator($field, $label, $confirm, $confirmLabel));
		}
	}
	

	function format($field)
	{
		return StringTypeRenderer::format($this->parent->data->get($field), $this->template);
	}
		
}

?>