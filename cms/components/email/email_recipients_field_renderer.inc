<?php
/**************************************************************

 Copyright (c) 2010 Sonjara, Inc

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

Fakoli::usingFile("framework/field_renderers.inc");

/*
 * For forms that use a field "recipients" for entry of
 * multiple email addresses. If the user enters inconsistent
 * delimiters between email addresses, change to consistent ","
 * on preprocess.
 */
class EmailListFieldRenderer extends TextFieldRenderer
{
	function EmailListFieldRenderer(&$form, $field, $label)
	{
		parent::TextFieldRenderer($form);
		
		if ($form->data->hasField($field))
		{
			$form->override($field, $label, $this);
		}
		else
		{
			$form->add($this, $field);
				$form->overrides[$field]['label'] = $label;
		}		
	}
	
	function addValidatorsToForm($field, $required = false)
	{
		$label = isset($this->label) ? $this->label : $this->parent->prettifyFieldName($field);
		
		$this->parent->validator->add(new EmailListValidator($field, $label));
		
		if($required)
		{
			$this->parent->validator->add(new RequiredValidator($field, $label));
		}
	}
	
	
	function renderField($field)
	{
		$recipients = $this->parent->data->get($field);
		$recipients = preg_replace("/,\s*/", ", ", $recipients);
		$this->parent->data->set($field, $recipients);
		
		parent::renderField($field);
	}
	
	function preProcess($field = "")
	{
		if(!$field) $field = "recipients";
		
		$recipients = $this->parent->data->get($field);

		$patterns = array("/,\s+/", "/;\s+/");
		$replacements = array(",", ",");
		$recipients =  preg_replace($patterns, $replacements, $recipients);

		$this->parent->data->set($field, $recipients);
	}	
}