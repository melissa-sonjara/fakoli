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
 * SelectFieldRenderer: Renders the specified list of options as a dropdown select box
 * 
 * @param obj $form the AutoForm object, passed by reference
 * 
 * @param string $field the name of the field defined in the dataitem class object 
 * 		   in the datamodel folder whose value can be changed by the select 
 * 		   item list box. This field name should match the field name in the 
 * 		   database.
 * 
 * @param string $label presented on the form to the user to describe the list box
 * 
 * @param array options defined in the dataitem class object that contains the 
 * 		   option ids and labels for the list box 
 * 		   For example, for action_type, create array ActionTypes as follows:
 * 			  actionTypes = array (
 * 					1	=>	"Introduction",
 *					2	=>	"Investigation",
 *					3	=>	"Formal Assessment"
 * 					)
 * 
 * @param string $onChange (optional) name of function in the calling 
 * 			script to be called when the user changes the selection.
 * 		    For example, one or more fields needs to be disabled/enabled 
 *			depending on the user's selection.
 * 
 */
class SelectFieldRenderer extends FieldRenderer
{
	var $options;		///< The array of options (ungrouped or grouped) that will displayed in the select box
	var $addEntry;		///< Set to true to show an "Add New" button
	var $addEntryText;	///< Text for the "Add New" button
	var $onChange; 		///< optional selection change handler
	var	$max_chars;		///< Maximum number of characters to display
	var $width;			///< Width of the select box, using CSS units (px, %, em, etc)

	var $defaultBlank = false; 		 	///< Whether the drop down includes an empty line as the default, indicating no selection has been made
	var $defaultText = "Choose One";	///< Text to display when no selection has been made.
	var $defaultValue = "";				///< The value for the default item - usually blank but might need to be zero for numeric fields
	var $allowDeselect = false;			///< If set to true, allows users to deselect the value for a field, returning it to the default 'blank' state
	
	var $CSSclass;		///< Additional CSS class(es) to apply to the select box
	var $stripHTML = false; ///< Set to true to strip HTML from option text
	
	function SelectFieldRenderer(&$form, $field, $label, $options = null, $onChange="", $grouped = false)
	{
		$this->options = $options;
		$this->onChange = $onChange;
		$this->grouped = $grouped;
		$this->FieldRenderer($form);
		if ($form->getData()->hasField($field))
		{
			// JDG 5/30/11 - don't crash on additiona/psuedo fields
			// only query if real field
			$form->override($field, $label, $this);
			if ($this->options == null)
			{
				$opt = $form->getData()->distinctValues($field, true);
				if (count($opt) > 0)
				{
					$this->options = array_combine($opt, $opt);
				}
				else
				{
					$this->options = array();
				}
			}
		}
		else
		{
			$form->add($this, $field);
			// JDG 7/22/2011 - allow "additional" fields to override label
			$form->overrides[$field]['label'] = $label;
		}
		
	}
	
	/**
	 * Sets the maximum number of characters to display in the select box (per entry)
	 * @param int $max the maximum number of characters to display in the select box
	 */
	function setMaxChars($max)
	{
		$this->max_chars = $max;
		return $this;
	}
	
	/**
	 * Enables the "Add New" button that allows users to create new options when
	 * the option they require is not present.
	 * @param string $text the text to display next to the Add New button (optional)
	 */
	function allowAddEntry($text = "or add new")
	{
		$this->addEntry = true;
		$this->addEntryText = $text;
		$this->addOtherOption();
		return $this;
	}
	
	/**
	 * Allow default blank selections.
	 * @param string $text the option text for the default blank selection.
	 */
	function allowBlank($text = "", $value = "")
	{
		if (!$this->grouped && !$text) $text = "Choose One";
		$this->defaultBlank = true;
		$this->defaultText = $text;
		$this->defaultValue = $value;
		return $this;
	}
	
	/**
	 * If no blank exists in the options and the user has not already
	 * saved a value, add a blank as the first option.
	 * 
	 * @param $field
	 */
	function addDefaultOption($field)
	{
		if(!$this->defaultBlank || ($this->parent->data->get($field) && !$this->allowDeselect)) return;
		
		if(count($this->options) && !array_search("", $this->options))
		{
			// want the blank at the top - array_merge causes issues; copy array instead
			$options[$this->defaultValue] = $this->defaultText;
			foreach($this->options as $key => $value)
			{
				$options[$key] = $value;
			}
			$this->options = $options;
		}
	}

	/**
	 * Extends the options using the specified array. Use this to provide
	 * a default set of options for SelectFieldRenderers that auto-populate
	 * based on field content.
	 */
	function extendOptions($options)
	{
		if ($this->grouped)
		{
			throw new FakoliException("SelectFieldRenderer::extendOptions() not supported in grouped mode");
		}
		
		foreach($options as $value => $text)
		{
			if (!array_key_exists($value, $this->options))
			{
				$this->options[$value] = $text;
			}
		}
	}
	
	function renderField($field)
	{
		$this->_startField($field);
			
		if (count($this->options) > 0 || !$this->addEntry)
		{			
			$cssClass = $this->CSSclass ? " class='{$this->CSSclass}'" : "";
			if($this->width)
				$style = "style='width: {$this->width}'";
			echo "<select {$class}{$style} name='$field' id='{$this->parent->id}_{$field}'";
			
			if ($this->onChange != "")
				echo " onchange='$this->onChange(this)'";
			elseif($this->addEntry)
				echo " onchange='onChange_{$this->parent->id}_{$field}(this)'";
			
			echo ">\n";
			
			/* Check if we have addEntry text coming in from the db;
			 * othEntry is text that doesn't match the set of options
			 * for this field
			 * */
			$othEntry = true;
			$current = $this->parent->data->get($field);
			
			if ($this->parent->data->getType($field) == 'String')
			{
				$comparator = function($a, $b) { return (strcmp($a, $b) == 0);	};
			}
			else
			{
				$comparator = function($a, $b) { return $a == $b; };
			}
			
			if ($this->grouped)
			{
				if ($this->defaultBlank)
				{
					echo "<option value=''>{$this->defaultText}</option>\n";
				}
				
				//$this->addDefaultOption($field);	
				
				foreach($this->options as $groupName => $group)
				{
					if ($groupName == "")
					{
						//echo "<option value=''>{$this->defaultText}</option>\n";
						continue;
					}
					
					echo "<optgroup label=\"$groupName\">\n";
					
					foreach($group as $value => $name)
					{
						$name = ($this->stripHTML) ? stripHTML($name) : htmlSafe($name);
						$trunced = $this->max_chars ? ellipsis($name, $this->max_chars, true) : $name;
						
						if ($comparator($current, $value))
						{
							$selected = " selected";
							$othEntry = false;
						}
						else
							$selected = "";
						
						trace("$current == $value ".($current==$value)." $selected", 3);
						
						$optionTitle = ($name != $trunced) ? " title='$name'" : "";
						
						echo "<option$optionTitle value='".htmlSafe($value)."'$selected>$trunced</option>\n";
					}
					
					echo "</optgroup>\n";
				}
			}
			else
			{	
				/*
				 * If there is no match but there is a value saved, 
				 * (user entered an addEntry value) then set current 
				 *  to blank so that the "Other" option is shown as selected. 
				 */		
				if(!array_key_exists($current, $this->options))
				{
					$addEntryValue = $current;
					$current = "";
				}
			
				$this->addDefaultOption($field);	
		
				foreach($this->options as $value => $name)
				{
					$name = ($this->stripHTML) ? stripHTML($name) : htmlSafe($name);
					$trunced = $this->max_chars ? ellipsis($name, $this->max_chars, true) : $name;
					
					if($comparator($current, $value))
					{
						$selected = " selected";
					}
					else
						$selected = "";
						
					$optionTitle = ($name != $trunced) ? " title='$name'" : "";
					
					echo "<option$optionTitle value='".htmlSafe($value)."'$selected>$trunced</option>\n";
				}
			}
			
			echo "</select>\n";
		}
		
		/*
		 * We need to create this field whether displayed or not so that the
		 * custom required validator, SelectFieldRequiredValidator, works for 
		 * either case.
		 */
		$type = $this->addEntry ? 'text' : 'hidden';
		
		if ($this->addEntry)
		{
			echo "&nbsp;{$this->addEntryText}&nbsp;";
			$onChange = " onchange='onChange_{$this->parent->id}_{$field}_addEntry(this)'";
			
		}	
		echo "<input type='$type' name='{$field}_addEntry' id='{$field}_addEntry' value='{$addEntryValue}' size='20' $onChange/>";
		
		$this->_endField($field);
	}
	
	function renderSearchField($field, $mode, $value = "")
	{
		if ($mode != "equal" && $mode != "member") $mode = "equal";
		
		$searchValue = (!$value) ? $this->parent->params->get($field, $mode) : $value;
		
		$this->_startField($field);

		if($this->width)
		{
			$style = "style='width: {$this->width}'";
		}
		
		echo "<select $style id='$field' name='$field:$mode'";
		
		if ($this->onChange != "")
			echo " onchange='$this->onChange(this)'";
		
		echo "><option value=''></option>\n";
		foreach($this->options as $value => $name)
		{
			$selected = ($searchValue === $value) ? " selected" : "";
			$name = ($this->stripHTML) ? stripHTML($name) : htmlSafe($name);
				
			echo "<option value='".htmlSafe($value)."'$selected>$name</option>";
		}
		echo "</select>\n";
		
		$this->_endField($field);
	}
	
	
	function renderReadOnly($field)
	{
		$this->_startField($field);

		$value = $this->parent->data->get($field);
		if ($this->grouped)
		{
			$found = false;
			foreach($this->options as $group => $options)
			{
				if (array_key_exists($value, $options))
				{
					echo $options[$value];
					$found = true;
					break;
				}
			}
			if (!$found)
			{
				echo "Not specified";
			}
		}
		else
		{
			if (array_key_exists($value, $this->options))
			{
				echo $this->options[$value];
			}
			elseif($value)
			{
				echo $value;
			}
			else
			{
				echo "Not specified";
			}
		}
		
		$this->_endField($field);
	}
	
	function preProcess($field = "")
	{
		$found = false;
		
		$current = $this->parent->data->get($field);

		if ($this->grouped)
		{
			foreach($this->options as $groupName => $group)
			{
				foreach($group as $value => $name)
				{
					if($current == $value && $value != '')
						$found = true;
				}
			}
		}
		else
		{
			foreach($this->options as $value => $name)
			{
				if($current == $value && $value != '')
					$found = true;
			}
		}

		$addEntry = $_POST["{$field}_addEntry"];
		trace("addEntry($field): $addEntry and found $found", 3);
		
		if ($_POST["{$field}_addEntry"] && !$found)
		{
			trace("setting field $field with add entry value $addEntry", 3);
			$this->parent->data->set($field,  $addEntry);
		}
	}
	
	/**
	 * If the calling script is using addEntry and
	 * does not supply its own onChange function,
	 * then write this onChange function to remove
	 * the added entry when the selected value in
	 * the drop down is NOT "Other". If the calling
	 * script requires additional onChange
	 * handling, it can override and incorporate this
	 * handling into its own custom function.
	 */
	function renderScript($field)
	{
		$fn = "{$this->parent->id}_$field";
		$addEntry = $field . "_addEntry";
		
		if(!$this->onChange && $this->addEntry)
		{
?>
<script type="text/javascript">
function onChange_<?echo $fn?>(elt)
{
	if(!elt.value == '')
	{
		var addEntry = document.id('<?php echo $addEntry ?>');
		if(addEntry)
			addEntry.value = '';		
	}
}
</script>
<?
		} // end if onChange and addEntry

		if($this->addEntry)
		{
?>
	<script type="text/javascript">
function onChange_<?echo "{$fn}_addEntry" ?>(elt)
{
	if(elt.value)
	{
		var selected = document.id('<?php echo $field ?>');
		if(selected)
			selected.value = '';		
	}
}
</script>	
<?php	
		} // ent if this addEntry		
	} // end function renderScript
	
	
	/**
	 * If the user enables addEntryText, then add a "Other" option to the 
	 * options array if one does not already exist. Can also add a 
	 * blank option.
	 */
	function addOtherOption($text = "Other")
	{
		if(!array_key_exists("", $this->options))
		{
			$this->options[""] = $text;
		}	

		return $this;
	}

	
} // end class SelectFieldRenderer

?>