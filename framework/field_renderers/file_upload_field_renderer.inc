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
 * Field renderer for file upload data fields. 
 * Renders using the browser's standard file upload control.
 * The programmer must supply a callback handler to manage the
 * file upload process, as upload logic can vary greatly from
 * application to application. 
 */
class FileUploadFieldRenderer extends FieldRenderer
{
	var $handler;
	var $size;
	var $field;
	
	function FileUploadFieldRenderer(&$form, $field, $label, $handler)
	{
		$this->FieldRenderer($form);
		
		$this->handler = $handler;
		$this->field = $field;
		
		if ($form->data->hasField($field))
		{
			$form->override($field, $label, $this);
		}
		else
		{
			$form->add($this, $field);
			$form->overrides[$field]['label'] = $label;		
		}
				
		$size = 40;
	}
	
	function renderField($field)
	{
		$this->_startField($field);
		echo "<input id='{$this->parent->id}_{$field}' type='file' name='$field' value='".$this->parent->data->get($field)."' size='{$this->size}'>";
		$this->_endField($field);
	}
	
	function renderReadOnly($field)
	{
		echo "<tr>\n";
		$this->_printLabel($field);
		echo "<td";
		if ($this->parent->valueCSS) echo " class='{$this->parent->valueCSS}'";
		echo ">{$this->parent->data->$field}</td>\n";
		echo "</tr>\n";
	}
	
	function preProcess($field)
	{
		call_user_func($this->handler, $field, $this->parent->data);
	}

	function required($message = "")
	{
		$this->parent->validator->add(new RequiredFileValidator($this->field, $this->label, $message));
	}
}

?>