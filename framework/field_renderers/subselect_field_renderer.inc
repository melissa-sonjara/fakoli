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
 * SubSelectFieldRenderer - provides a second-level hierarchical selection
 * mechanism based on the results of another select-based field renderer 
 * (such as SelectFieldRenderer or RelatedItemSelectFieldRenderer)
 * @author andy
 */
class SubSelectFieldRenderer extends FieldRenderer
{
	var $super;
	var $options;
	var $format;
	
	function SubSelectFieldRenderer(&$form, $field, $label, $super, $options, $format)
	{
		$this->options = $options;
		$this->super = $super;
		$this->format = $format;
		$this->FieldRenderer($form);
		if ($form->data->hasField($field))
		{
			// JDG 5/30/11 - don't crash on additiona/psuedo fields
			// only query if real field
			$form->override($field, $label, $this);			
		}
		else
		{
			$form->add($this, $field);
			// JDG 7/22/2011 - allow "additional" fields to override label
			$form->overrides[$field]['label'] = $label;
		}
	}
	
	/**
	 * Override this function to provide extra editing controls that follow the visibility
	 * rules of the main control.
	 * @param string $field
	 */
	function extraControls($field)
	{
		
	}
	
	function renderField($field)
	{
		$this->_startField($field);
			
		if($this->width)
		{
			$style = "style='width: {$this->width}'";
		}

		echo "<span class='subselect_container'>";
		echo "<select $style name='$field' id='{$this->parent->id}_{$field}'";
		
		if ($this->onChange != "")
		{
			echo " onchange='$this->onChange(this)'";
		}
			
		echo ">\n";			
		echo "</select>\n";
		
		$this->extraControls($field);
		
		echo "</span>";
		
		$this->_endField($field);
	}
	
	function renderSearchField($field, $mode, $value = "")
	{
		if ($mode != "equal") $mode = "equal";
		
		$searchValue = (!$value) ? $this->parent->params->get($field, $mode) : $value;
		
		$this->_startField($field);
		
		echo "<select id='$field' name='$field:$mode'><option value=''></option>\n";
		foreach($this->options as $value => $name)
		{
			$selected = ($searchValue == $value) ? " selected" : "";
			
			echo "<option value='".htmlSafe($value)."'$selected>$name</option>";
		}
		
		echo "</select>";
		
		$this->_endField($field);
	}
	
	
	function renderReadOnly($field)
	{
		$super_id = $this->parent->data->get($this->super);
		
		if (!array_key_exists($super_id, $this->options)) return;
		
		$this->_startField($field);
		
		$item = findItem($this->options[$super_id], $field, $this->parent->data->get($field));
		
		if ($item)
		{
			echo $item->format($this->format);
		}
		else
		{
			echo "Not specified";
		}
		
		$this->_endField($field);
	}

	function renderScript($field)
	{
		if ($this->parent->readOnlyForm || $this->parent->isReadOnly($field)
			|| array_key_exists($field, $this->parent->hidden)) return "";

		$mgr = "{$this->parent->id}_{$field}_mgr";
		$super_id = "{$this->parent->id}_{$this->super}";
		$value = $this->parent->data->get($field);
		$this->setFormatTemplate();
?>
<script type="text/javascript">
window.addEvent('domready', function()
{
	var <?echo $mgr?> = new SubSelectManager('<?echo $super_id?>', '<?echo $this->parent->id?>_<?echo $field?>', '<?echo $value?>');
<?
		foreach($this->options as $key => $options)
		{
			$pk = $options[0]->getPrimaryKey();			
			$values = formatItems($options, "{'name': '{SubSelectFieldRenderer::formatOption}', 'value': {{$pk}} }", ", ");
?>
	<?echo $mgr?>.addOptions(<?echo $key?>, [{'name': '', 'value': 0}, <?echo $values?>]);
<?
		}
?>
	<?echo $mgr?>.update();
});
</script>
<?
	} 
	
	/**
	 * We need to save the class var format into the object for
	 * use in the static call to formatOption. We can't call
	 * formatOption using the class instance $this through the
	 * formatItems function.
	 */
	function setFormatTemplate()
	{
		foreach($this->options as $key => $options)
		{
			foreach($options as $option)
			{
				$option->set("_formatTemplate", $this->format);
			}
		}	
	}	
	
	static function formatOption($option)
	{
		$template = $option->get("_formatTemplate");
		return jsSafe($option->format("{$template}"));
	}	
			
	function preProcess($field = "")
	{
		if(!$this->parent->data->hasField($this->super)) return;
		
		$super_id = $this->parent->data->get($this->super);
		
		if (!array_key_exists($super_id, $this->options))
		{
			$this->parent->data->set($field, 0);
		}		
	}
	
} // end class SubSelectFieldRenderer

/**@}*/?>