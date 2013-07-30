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
 NONINFRINGEMENT. IN NO EVENT SHALL  THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.

*****************************************************************/

require_once realpath(dirname(__FILE__))."/../field_renderers.inc";

class URLFieldRenderer extends StringFieldRenderer
{
	function URLFieldRenderer(&$parent)
	{
		$this->StringFieldRenderer($parent);
	}
	
	function addValidatorsToForm($field, $required = false)
	{
		$label = isset($this->label) ? $this->label : $this->parent->prettifyFieldName($field);
	
		$this->parent->regexp($field, "^(ht|f)tp(s?)\\:\\/\\/[0-9a-zA-Z]([-.\\w]*[0-9a-zA-Z])*(:(0-9)*)*(\\/?)([a-zA-Z0-9\\-\\.\\?\\,\\'\\/\\\\\\+&amp;%\$#_]*)?$", "Please supply a valid URL.");
		
		if($required)
		{
			$this->parent->validator->add(new RequiredValidator($field, $label));
		}
	}
	
	function renderReadOnly($field)
	{
		$this->_startField($field);
	
		$value = $this->parent->data->get($field);
		$escaped = htmlspecialchars($value, ENT_QUOTES, 'UTF-8');
		$safe = htmlSafe($value);
		
		if (!$this->parent->readOnlyForm)
		{
			echo "<input id='{$this->parent->id}_{$field}' type='hidden' name='$field' value='".$escaped."'/>";
		}
	
		if ($value)
		{
			echo "<a href='$value'>$safe</a>";
		}
		else
		{
			echo htmlSafe($value);
		}
		
		$this->_endField($field);
	}
	
}

?>