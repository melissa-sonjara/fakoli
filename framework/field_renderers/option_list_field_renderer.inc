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
 * Displays a control that provides a highly styleable set of options from which the user can select a single item.
 * The optons are provided as an array of value-description pairs, and are rendered as an unordered 
 * list in HTML, with javascript to provide the selection interaction. Clicking on one of the items
 * will select the associated value. The currently selected list item is given the "selected" CSS class.
 * Options can also be provided grouped as an associative array with the group headings as the keys.
 * 
 * @author Andy
 */
class OptionListFieldRenderer extends FieldRenderer
{
	var $options = array();			///< The array of options for the control to display, as value=>description pairs.
	var $cssClass = "options_list";	///< CSS class for the generated list of options. Use this to override the look and feel
	var $grouped = false;			///< True if the $options array is grouped by headings, false if not
	var $groupTag = "h6";			///< The tag to use when displaying the groups.
	
	function OptionListFieldRenderer(&$form, $field, $label, $options, $grouped = false)
	{
		$this->options = $options;
		$this->grouped = $grouped;
		
		$this->FieldRenderer($form);
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
		value = sel.get('data-value');
		
		$$('#<?echo $id?>_list li[data-value]').each(function(elt) 
		{ 
			var v = elt.get('data-value');
			if (v == value)
			{
				elt.addClass('selected');
			}
			else
			{
				elt.removeClass('selected'); 
			}
		});

		var hidden = document.id('<?echo $id?>');
		var v = hidden.value;
		hidden.value = value;
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
		
		$id = "{$this->parent->id}_{$field}";
		
		echo "<input type='hidden' id='{$id}' name='{$field}' value='{$value}'/>";
		if (!$this->grouped)
		{
			echo "<ul id='{$id}_list' class='{$this->cssClass}'>";
			foreach($this->options as $option => $description)
			{
				$selected = ($value == $option) ? " class='selected'" : "";
				echo "<li{$selected} data-value='{$option}' onclick='{$id}_update(this); return false;'>{$description}</li>\n";	
			}
			echo "</ul>";
		}
		else
		{
			echo "<div id='{$id}_list' class='{$this->cssClass}'>";
			
			foreach($this->options as $group => $options)
			{
				echo "<{$this->groupTag}>{$group}</{$this->groupTag}>\n";
				echo "<ul class='{$this->cssClass}'>";
				foreach($options as $option => $description)
				{
					$selected = ($value == $option) ? " class='selected'" : "";
					echo "<li{$selected} data-value='{$option}' onclick='{$id}_update(this); return false;'>{$description}</li>\n";
				}
				echo "</ul>";
			}
			
			echo "</div>";
		}
		
		$this->_endField($field);
	}
	
	function renderReadOnly($field)
	{
		$this->_startField($field);
		
		$value = $this->parent->data->get($field);
		if ($value)
		{
			echo $this->options[$value];	
		}
		$this->_endField($field);
	}
}

/**@}*/