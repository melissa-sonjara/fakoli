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
 * Field renderer for data fields that must be displayed as a list
 * of choices. Renders as a set of buttons from which the user
 * can choose one or more responses.
 */
class PushButtonCheckListFieldRenderer extends FieldRenderer
{
	var $options;
	var $horizontal = false;
	var $onChange = ""; // optional callback on data input change
	var $disable = false; // alternative to readonly; field can be reenabled thru javascript
	var $tabular = false; // Horizontal table-based layout
	var $regular = false;
	
	function PushButtonCheckListFieldRenderer(&$form, $field, $label, $options)
	{
		$this->options = $options;
				
		$this->FieldRenderer($form);
		if ($form->data->hasField($field))
		{
			$form->override($field, $label, $this);
		}
		else
		{
			$form->add($this, $field);
			$form->alias($field, $label);
		}
	}

	function addValidatorsToForm($field, $required = false)
	{
		$label = isset($this->label) ? $this->label : $this->parent->prettifyFieldName($field);
		
		// JDG 2/2012 - changed from tree validator expects a div and does not match the rendering of this type
		if($required)
		{
			$this->parent->validator->add(new RequiredValidator($field, $label));
		}
		
	}
	

	function renderScript($field)
	{
		$fn = "{$this->parent->id}_{$field}_update";
		$containerId = "{$this->parent->id}_{$field}_container";
		$id = "{$this->parent->id}_{$field}";
?>
<script type='text/javascript'>
function <?echo $fn?>(element)
{
	var container = document.id('<?echo $containerId?>');

	element = document.id(element);
	if (element.hasClass('selected'))
	{
		element.removeClass('selected');
	}
	else 
	{
		element.addClass('selected');
	}

	var values = [];
		
	container.getElements("a.selected").each(function(elt)
	{
		values.push(elt.get('data-value'));
	});

	document.id('<?echo $id?>').value = values.join(",");
	return false;
}
</script>
<?		
	}
	
	function renderField($field)
	{
		if (!count($this->options)) return;
		
		$this->_startField($field);
		
		$this->renderButtons($field);
		
		
		$this->_endField($field);
	}

	function renderButtons($field, $mode = null)
	{
		$current = $this->parent->data->get($field);
		
		$separator = "";
		
		if ($this->regular && !$this->tabular)
		{
			$width = 100 / count($this->options);
			$widthStyle = " style='width: {$width}%'";
		}
		
		$containerId = "{$this->parent->id}_{$field}_container";
		
		$values = explode(",", $this->parent->data->get($field));
		$values = array_combine($values, $values);
		
		if ($this->tabular)
		{
			if ($this->regular)
			{
				$width = 100 / count($this->options);
				$tdStyle = " style='width: {$width}%'";
			}
			else
			{
				$tdStyle = "";
			}
		
			echo "<table id='{$containerId}' class='push_button_options'><tr><td{$tdStyle}>";
			$separator = "</td><td{$tdStyle}>";
			$close = "</td></tr></table>";
		}
		else if ($this->horizontal)
		{
			echo "<div id='{$containerId}' class='push_button_options horizontal'>";
			$close = "</div>";
		}
		else
		{
			echo "<div id='{$containerId}' class='push_button_options vertical'>";
			$close = "</div>";
		}
		
		$name = $mode ? "$field:$mode" : $field;
		
		echo "<input type='hidden' id='{$this->parent->id}_{$field}' name='{$name}' value='$current'/>";
	
		$out = array();
	
		$max = count($this->options) - 1;
		$disable = ($this->disable) ? "disabled='disabled'" : "";
		
		$count = 0;
		foreach($this->options as $value => $name)
		{
			$selected = (array_key_exists($current, $values)) ? " selected" : "";
			$onchange = ($this->onChange) ? "{$this->onChange}(this);" : "";
	
			$position = "";
			if ($count == 0) $position .= " first";
			if ($count == $max) $position .= " last";
				
			// JDG 3/34/10, add border: none to correct IE8 display issue
			$out[] = "<a class='pushbutton{$position}{$selected}'{$widthStyle} href='#' data-value='{$value}' onclick='{$this->parent->id}_{$field}_update(this);{$onChange}return false' {$disable}/>{$name}</a>";
	
			$count++;
		}

		$buttons = implode($separator, $out);
		$buttons .= $close;
		
		echo $buttons;
	}
	
	function renderSearchField($field, $mode)
	{
		if ($mode != "equal") return;
		
		$searchValue = $this->parent->params->get($field, $mode);
	
		$this->_startField($field);
		
		$this->renderButtons($field, $mode);
		
		$this->_endField($field);
	}
	
	
	function renderReadOnly($field)
	{
		$this->_startField($field);

		if (array_key_exists($this->parent->data->get($field), $this->options))
		{
			echo $this->options[$this->parent->data->get($field)];
		}
		else
		{
			echo "Not specified";
		}
		$this->_endField($field);
	}
}


?>