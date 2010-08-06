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
 * Field renderer for date data fields. Renders with date format
 * validation and a date picking popup widget.
 */
class DateFieldRenderer extends FieldRenderer
{
	var $validator;
	var $required = true;
	
	function DateFieldRenderer(&$parent)
	{
		$this->FieldRenderer($parent);
		$this->validator = new DateValidator("", "", $required);
	}
	
	function addValidatorsToForm($field)
	{
		$label = isset($this->label) ? $this->label : $this->parent->prettifyFieldName($field);

		$this->validator->field = $field;
		$this->validator->title = $label;
		
		$this->parent->validator->add($this->validator);
	}
	
	function renderScript($field)
	{
		$script = "";
		
		if (!$this->_includedDateScript)
		{
			echo "<script src='{$this->parent->componentPath}/calendar/sonjara_calendar.js' type='text/javascript'></script>\n";
			echo "<link href='{$this->parent->componentPath}/calendar/sonjara_calendar.css' type='text/css' rel='stylesheet'>\n";
			$this->_includedDateScript = true;
		}
		
		$calendar = "{$field}_calendar";
		echo "<script type='text/javascript'>\n";
		echo "var $calendar = new Calendar('$calendar', '{$this->parent->id}', '$field');\n";
		echo "</script>\n";
	}

	function renderField($field)
	{
		$obj = $this->parent->data;
		$date = $obj->reformatFromSQLDate($obj->get($field));
		
		$this->_startField($field);
		echo "<input id='{$this->parent->id}_{$field}' type='text' name='$field' value='$date' size='12'>&nbsp;<img src='{$this->parent->componentPath}/calendar/calendar.gif' alt='Popup Calendar' align='absmiddle' border='0' onclick='{$field}_calendar.toggle(this)'>\n";
		$this->_endField($field);
	}
	
	function renderSearchScript($field, $mode)
	{
		$script = "";
		
		if (!$this->_includedDateScript)
		{
			echo "<script src='{$this->parent->componentPath}/calendar/sonjara_calendar.js' type='text/javascript'></script>\n";
			echo "<link href='{$this->parent->componentPath}/calendar/sonjara_calendar.css' type='text/css' rel='stylesheet'>\n";
			$this->_includedDateScript = true;
		}
		
		if ($mode == "range")
		{
			$calendar_from = "{$field}_calendar_from";
			$calendar_to = "{$field}_calendar_to";
			
			echo "<script type='text/javascript'>\n";
			echo "var $calendar_from = new Calendar('$calendar_from', '{$this->parent->id}', '$field:from');\n";
			echo "var $calendar_to = new Calendar('$calendar_to', '{$this->parent->id}', '$field:to');\n";
			echo "</script>\n";
		}
		else
		{
			$calendar = "{$field}_calendar_{$mode}";
			echo "<script type='text/javascript'>\n";
			echo "var $calendar = new Calendar('$calendar', '{$this->parent->id}', '$field:$mode');\n";
			echo "</script>\n";
		}
	}
	
	function renderSearchField($field, $mode)
	{
		if ($mode == "range")
		{
			$date_from = $this->parent->params->get($field, "from");
			$date_to = $this->parent->params->get($field, "to");
			echo "<tr>\n";
			$this->_printLabel($field);
			echo "<td>";
			echo "<input type='text' name='$field:from' value='$date_from' size='12'>&nbsp;<img src='{$this->parent->componentPath}/calendar/calendar.gif' alt='Popup Calendar' align='absmiddle' border='0' onclick='{$field}_calendar_from.toggle(this)'>";
			echo " to <input type='text' name='$field:to' value='$date_to' size='12'>&nbsp;<img src='{$this->parent->componentPath}/calendar/calendar.gif' alt='Popup Calendar' align='absmiddle' border='0' onclick='{$field}_calendar_to.toggle(this)'></td>\n";
			echo "</td>\n";
			echo "</tr>\n";	
		}
		else
		{
			$date = $this->parent->params->get($field, $mode);
			echo "<tr>\n";
			$this->_printLabel($field);
			echo "<td>";
			echo "<input type='text' name='$field:$mode' value='$date' size='12'>&nbsp;<img src='{$this->parent->componentPath}/calendar/calendar.gif' alt='Popup Calendar' align='absmiddle' border='0' onclick='{$field}_calendar_{$mode}.toggle(this)'></td>\n";
			echo "</td>\n";
			echo "</tr>\n";			
		}
	}
	
	function renderReadOnly($field)
	{
		echo "<tr>\n";
		$this->_printLabel($field);
		echo "<td";
		if ($this->parent->valueCSS) echo " class='{$this->parent->valueCSS}'";
		echo ">".$this->parent->data->get($field)."</td>\n";		
		echo "</tr>\n";	
	}	
}

?>