<?php
/**\addtogroup fieldrenderers
 * @{
 */

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

/**
 * Displays a control that provides a highly styleable set of options from which the user can select a number of items.
 * The optons are provided as an array of value-description pairs, and are rendered as an unordered
 * list in HTML, with javascript to provide the selection interaction. Clicking on one of the items
 * will select the associated value. The currently selected list item is given the "selected" CSS class.
 *
 * @author Andy
 */
class OptionCheckListFieldRenderer extends FieldRenderer
{
	var $options = array();
	var $cssClass = "options_list";
	var $separator = ", ";
	
	function __construct(&$form, $field, $label, $options)
	{
		$this->options = $options;
		parent::__construct($form);
		if ($form->data->hasField($field))
		{
			$form->override($field, $label, $this);
		}
		else
		{
			$form->add($this, $field);
			if ($label) $form->alias($field, $label);
		}
	}


	function renderScript($field)
	{
		if ($this->parent->readOnlyForm || $this->parent->isReadOnly($field)) return;
		$id = "{$this->parent->id}_{$field}";
		?>
	<script type='text/javascript'>
	function <?echo $id?>_update(sel)
	{
		if (sel.hasClass("selected"))
		{
			sel.removeClass("selected");
		}
		else
		{
			sel.addClass("selected");
		}
		
		var values = [];
		
		$$('#<?echo $id?>_list > li').each(function(elt) 
		{ 
			if (elt.hasClass('selected'))
			{
				var v = elt.get('data-value');
				values.push(v);
			}
		});

		var hidden = document.id('<?echo $id?>');
		var v = hidden.value;
		hidden.value = values.join(",");
		var f = document.id('<?echo $this->parent->id?>');
		if (v != hidden.value && f && f.manager)
		{
			f.manager.dirty();
		}
	}
	</script>
<?
	}
	
	function renderField($field)
	{
		$this->_startField($field);
				
		$value = $this->parent->data->get($field);
		
		$values = explode(",", $value);
		$values = array_combine($values, $values);
		
		$id = "{$this->parent->id}_{$field}";
		
		echo "<input type='hidden' id='{$id}' name='{$field}' value='{$value}'/>";
		echo "<ul id='{$id}_list' class='{$this->cssClass}'>";
		foreach($this->options as $option => $description)
		{
			$selected = ($values[$option] == $option) ? " class='selected'" : "";
			echo "<li{$selected} data-value='{$option}' onclick='{$id}_update(this); return false;'>{$description}</li>\n";	
		}
		echo "</ul>";
		$this->_endField($field);
	}
	
	function renderReadOnly($field)
	{
		$this->_startField($field);
		
		$text = array();
		$value = $this->parent->data->get($field);
		$values = explode(",", $value);
		
		foreach($values as $v)
		{
			$text[] = $this->options[$v];	
		}
		
		echo implode($this->separator, $text);
		
		$this->_endField($field);
	}
}

/**@}*/?>