<?php
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
 * Field renderer for date of birth data fields. Renders with date format
 * validation and separate dropdowns for month, day and year.
 */
class DateOfBirthFieldRenderer extends FieldRenderer
{
	var $validator;
	var $template = ""; // default reaonly format is short style

	function DateFieldRenderer(&$parent)
	{
		$this->FieldRenderer($parent);
	}

	function addValidatorsToForm($field, $required = false)
	{
		$label = isset($this->label) ? $this->label : $this->parent->prettifyFieldName($field);

		if($required)
		{
			$this->parent->validator->add(new RequiredValidator("{$field}_mm", "$label Month"));
			$this->parent->validator->add(new RequiredValidator("{$field}_dd", "$label Day"));
			$this->parent->validator->add(new RequiredValidator("{$field}_yy", "$label Year"));
		}

	}

	function renderScript($field)
	{
	}

	function renderField($field)
	{
		$obj = $this->parent->data;
		if ($obj instanceof CompositeDataItem)
		{
			$obj = $obj->findSubObject($field);
		}
		
		$month = $obj->format("{{$field}:m}");
		$day = $obj->format("{{$field}:d}");
		$year = $obj->format("{{$field}:Y}");
		
		$this->_startField($field);
		
		echo "<select id='{$this->parent_id}_{$field}_mm' type='text' name='{$field}_mm'><option value=''></option>";
		
		for($i = 1; $i <= 12; ++$i)
		{
			$mm = new DateTime("{$i}/1/1971");
			$monthName = $mm->format("M");
			option($i, $monthName, $month);
		}	

		echo "</select> / <select id='{$this->parent_id}_{$field}_dd' type='text' name='{$field}_dd'><option value=''></option>";
		
		for($i = 1; $i < 32; ++$i)
		{
			option($i, $i, $day);
		}
		
		echo "</select> / <select id='{$this->parent_id}_{$field}_yy' type='text' name='{$field}_yy'><option value=''></option>";
		
		for($i = 1900; $i < 2014; ++$i)
		{
			option($i, $i, $year);
		}
		
		echo "</select>";
		
		$this->_endField($field);
	}

	function renderSearchScript($field, $mode)
	{
	}

	function renderSearchField($field, $mode)
	{
	}

	function renderReadOnly($field)
	{
		$this->_startField($field);

		echo $this->format($this->parent->data->get($field));

		$this->_endField($field);
	}

	function format($value)
	{
		return DateTypeRenderer::format($value, $this->template);
	}
	
	function preProcess($field)
	{
		$mm = checkNumeric($_POST["{$field}_mm"]);
		$dd = checkNumeric($_POST["{$field}_dd"]);
		$yy = checkNumeric($_POST["{$field}_yy"]);

		if ($mm < 10) $mm = "0{$mm}";
		if ($dd < 10) $dd = "0{$dd}";
		
		$dob = "$yy-$mm-$dd";
		
		trace("** Preprocessing date of birth: $dob", 3);
		
		$this->parent->data->set($field, $dob);
	}
	
}

?>