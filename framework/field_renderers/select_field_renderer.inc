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
	var $options;
	var $addEntry;
	var $addEntryText;
	var $onChange; // optional selection change handler
	var	$max_chars;
	
	function SelectFieldRenderer(&$form, $field, $label, $options = null, $onChange="", $grouped = false)
	{
		$this->options = $options;
		$this->onChange = $onChange;
		$this->grouped = $grouped;
		$this->FieldRenderer($form);
		if ($form->data->hasField($field))
		{
			$form->override($field, $label, $this);
		}
		else
		{
			$form->add($this, $field);
		}
		
		if ($this->options == null)
		{
			$opt = $form->data->distinctValues($field, true);
			$this->options = array_combine($opt, $opt);
		}		
	}
	
	function allowAddEntry($text = "or add new")
	{
		$this->addEntry = true;
		$this->addEntryText = $text;
		$this->addOtherOption();
	}
	
	function renderField($field)
	{
		$this->_startField($field);
		
		if (count($this->options) > 0 || !$this->addEntry)
		{
			echo "<select name='$field' id='$field'";
			
			if ($this->onChange != "")
				echo " onchange='$this->onChange(this)'";
			
			echo ">\n";
			
			/* Check if we have addEntry text coming in from the db;
			 * othEntry is text that doesn't match the set of options
			 * for this field
			 * */
			$othEntry = true;
			$current = $this->parent->data->get($field);
			
			if ($this->grouped)
			{
				foreach($this->options as $groupName => $group)
				{
					echo "<optgroup label=\"$groupName\">\n";
					
					foreach($group as $value => $name)
					{
						if($current == $value)
						{
							$selected = " selected";
							$othEntry = false;
						}
						else
							$selected = "";
						
						echo "<option value='$value'$selected>$name</option>\n";
					}
					
					echo "</optgroup>\n";
				}
			}
			else
			{
				foreach($this->options as $value => $name)
				{
					if($current == $value)
					{
						$selected = " selected";
						$othEntry = false;
					}
					else
						$selected = "";
					
					echo "<option value='$value'$selected>$name</option>\n";
				}
			}
			
			if($othEntry)
				$addEntryValue = $this->parent->data->get($field);
			
			echo "</select>\n";
		}
		
		/*
		 * We need to create this field whether displayed or not so that the
		 * custom required validator, SelectFieldRequiredValidator, works for 
		 * either case.
		 */
		$type = $this->addEntry ? 'text' : 'hidden';
		
		if ($this->addEntry)
			echo "&nbsp;{$this->addEntryText}&nbsp;";
			
		echo "<input type='$type' name='{$field}_addEntry' id='{$field}_addEntry' value='{$addEntryValue}' size='20'/>";
		
		$this->_endField($field);
	}
	
	function renderSearchField($field, $mode, $value = "")
	{
		if ($mode != "equal") return;
		
		$searchValue = (!$value) ? $this->parent->params->get($field, $mode) : $value;
		echo "<tr>\n";
		$this->_printLabel($field);
		echo "<td><select id='$field' name='$field:$mode'><option value=''></option>\n";
		foreach($this->options as $value => $name)
		{
			$selected = ($searchValue == $value) ? " selected" : "";
			
			echo "<option value='$value'$selected>$name</option>";
		}
		echo "</select></td></tr>\n";
		
		echo "</select>";
	}
	
	
	function renderReadOnly($field)
	{
		echo "<tr>\n";
		$this->_printLabel($field);
		echo "<td";
		if ($this->parent->valueCSS) echo " class='{$this->parent->valueCSS}'";
		echo ">";
		if (array_key_exists($this->parent->data->get($field), $this->options))
		{
			echo $this->options[$this->parent->data->get($field)];
		}
		else
		{
			echo "Not specified";
		}
		echo "</td></tr>\n";
	}
	
	function preProcess($field)
	{
		$found = false;
		
		$current = $this->parent->data->get($field);
		
		foreach($this->options as $value => $name)
		{
			if($current == $value && $value != '')
			{
				$found = true;
			}
		}
					
		trace("addEntry($field): ".$_POST["{$field}_addEntry"], 3);
		
		if ($_POST["{$field}_addEntry"] && !$found)
		{
			$this->parent->data->set($field,  $_POST["{$field}_addEntry"]);
		}
	}
	
	/*
	 * addOtherOption
	 * 
	 * If the user enables addEntryText, then add a "Other" option to the 
	 * options array if one does not already exist. Sort by options key so 
	 * that the blank is on top. Can also add a blank option.
	 */
	function addOtherOption($text = "Other")
	{
		if(!array_key_exists("", $this->options))
		{
			$this->options[""] = $text;
		}
		
		ksort($this->options);
	}

	
} // end class SelectFieldRenderer

?>