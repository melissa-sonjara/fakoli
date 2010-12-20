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
 * Field renderer for boolean data fields. Renders as a checkbox.
 */
class BooleanFieldRenderer extends FieldRenderer
{
	function BooleanFieldRenderer(&$parent)
	{
		$this->FieldRenderer($parent);		
	}
	
	function addValidatorsToForm($field, $required = false)
	{
		$label = isset($this->label) ? $this->label : $this->parent->prettifyFieldName($field);
		
		if($required)
		{
			$this->parent->validator->add(new RequiredBooleanValidator($field, $label));
		}
		
	}
	
	
	function renderField($field)
	{		
		if ($this->annotateBefore && $this->parent->annotations[$field] && $this->colspan == 1)
		{
			echo "<tr><td>". $this->parent->annotations[$field] ."</td></tr>";
		}
		
		echo "<tr>\n";	
		echo "<td colspan='2' style='text-align: left'";
		if ($this->parent->labelCSS) echo " class='{$this->parent->labelCSS}'";
		echo "><input id='{$this->parent->id}_{$field}' type='checkbox' value='1' name='$field'";
		if ($this->parent->data->get($field))
		{
			echo " checked";
		}
		echo ">&nbsp;<label for='$field'>";
		echo $this->_getLabel($field, false);
		echo "</label>";
		if (!$this->annotateBefore && $this->parent->annotations[$field])
		{
			echo "<br/>{$this->parent->annotations[$field]}";
		}
		echo "</td>\n</tr>\n";
	}
	
	function renderSearchField($field)
	{
		echo "<tr>\n";
		echo "<td colspan='2'";
		if ($this->parent->labelCSS) echo " class='{$this->parent->labelCSS}'";
		echo "><input type='checkbox' value='1' name='$field:$mode'";
		if ($this->parent->data->get($field))
		{
			echo " checked";
		}
		echo ">&nbsp;";
		echo $this->parent->prettifyFieldName($field);
		echo "</td>\n</tr>\n";		
	}
	
	function renderReadOnly($field)
	{
		echo "<tr>\n";
		echo $this->_printLabel($field);
		echo "<td";
		if ($this->parent->valueCSS) echo " class='{$this->parent->valueCSS}'";
		echo ">";
		echo ($this->parent->data->get($field)) ? "Yes" : "No";
		echo "</td>\n</tr>\n";
	}
}

?>