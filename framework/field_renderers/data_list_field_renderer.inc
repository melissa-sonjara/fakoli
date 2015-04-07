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
 * Add a DataListView table to an AutoForm.
 * 
 * syntax:
 * 
 * $personSelect = new DataListFieldRenderer($form, $table);
 * 
 * where table is an object of class DataListView.
 * 
 * The $field is taken from the table id set when 
 * the obj is instantiated.
 * 
 * @author Janice Gallant for Sonjara, Inc.
 * 
 * 7/22/2011
 * 
 */
class DataListFieldRenderer extends FieldRenderer
{
	var $field;
	var $label;
	var $table;
	
	function DataListFieldRenderer(&$parent, $table, $field = "", $label = "")
	{
		$this->FieldRenderer($parent);

		$this->field = ($field) ? $field : $table->id;
		$this->label = ($label) ? $label : prettify($this->field);
				
		$this->table = $table;
		if ($parent->data->hasField($this->field))
		{
			$parent->override($this->field, $this->label, $this);
		}
		else
		{
			$parent->add($this, $this->field);	
			$form->overrides[$this->field]['label'] = $this->label;
		}
		$this->colspan = 2;
	}
	
	function renderScript($field)
	{
		return $this->table->writeScript();
	}
	
	function renderSearchField($field, $mode = "")
	{
		$this->renderField();
	}
	
	function renderField($field)
	{
		if (!$field) $field = $this->field;
		
		$this->_startField($field);
		$this->table->drawView();
		$this->_endField($field);
	}
	
	function preProcess($field = "")
	{
		if ($this->onPreProcess != null)
		{
			$callback = $this->onPreProcess;			
			call_user_func($callback, $this, $field);
		}
	}
	
	function renderReadOnly()
	{
		$this->table->selectable = false;
		$this->renderField();
	}
	
	function postProcess($field = "")
	{
		if ($this->onPostProcess != null)
		{
			$callback = $this->onPostProcess;
			call_user_func($callback, $this, $field);
		}
	}	
}