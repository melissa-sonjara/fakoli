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
 * 
 * @template - field formatting format for readonly is standard 
 * "F d, Y", accepts "long", "daylong", and "short" and any
 * format string used by php date function
 */
class DateTimeFieldRenderer extends FieldRenderer
{
	var $validator;
	var $requireTime = true; // whether the time component of the field is also required
	var $template = ""; // default reaonly format is short style
	
	function DateTimeFieldRenderer(&$parent)
	{
		$this->FieldRenderer($parent);
	}
	
	function addValidatorsToForm($field, $required = false)
	{
		$label = isset($this->label) ? $this->label : $this->parent->prettifyFieldName($field);
	
		$this->parent->validator->add(new DateValidator($field, $label));
		$this->parent->validator->add(new TimeValidator($field, $label));
			
		if($required)
		{
			$this->parent->validator->add(new RequiredValidator($field, $label));
			if($this->requireTime)
				$this->parent->validator->add(new RequiredTimeValidator($field, $label . " Time"));
		}	
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
		$value = $obj->get($field);
		
		// JDG 10/1/2010 check for empty saved string
		$date = "";
		if($value AND $value != '0000-00-00 00:00:00')
			$date = DateTimeTypeRenderer::format($obj->reformatFromSQLDate($value), "m/d/Y");
		
		$this->_startField($field);
		echo "<input id='{$this->parent->id}_{$field}' type='text' name='$field' value='$date' size='12'>&nbsp;<img src='{$this->parent->componentPath}/calendar/calendar.gif' alt='Popup Calendar' align='absmiddle' border='0' onclick='{$field}_calendar.toggle(this)'>\n";
			
		// If no time component, show as blank, not 12:00 AM
		if ($value AND !preg_match("/00:00:00$/", $value))
		{
			$text = date("h/i/A", strtotime($value));
			list($hh, $mm, $aa) = explode("/", $text);
		}
		
		echo "<div id='{$this->parent->id}_{$field}_block' style='display: inline-block'>\n";
		echo "&nbsp;&nbsp;<input type='text' value='$hh' name='{$field}_hh' size='2' maxlength='2' onkeydown='return maskInput(event);'>
		&nbsp;:&nbsp;<input type='text' value='$mm' name='{$field}_mm' size='2' maxlength='2' onkeydown='return maskInput(event);'>";
		echo "&nbsp;<select name='{$field}_aa'><option";
		if ($aa == 'AM') echo " selected";
		echo ">AM</option><option";
		if ($aa == 'PM') echo " selected";
		echo ">PM</option></select>";
		echo "</div>\n";
		
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
		echo ">".$this->format($field)."</td>\n";		
		echo "</tr>\n";	
	}
	
	function preProcess($field)
	{		
		$hh = checkNumeric($_POST["{$field}_hh"]);
		$mm = checkNumeric($_POST["{$field}_mm"]);
		$aa = $_POST["{$field}_aa"];
		$dd = $_POST["{$field}"];
		
		if (!$dd)
		{
			$this->parent->data->set($field, $this->parent->data->reformatToSQLDate("00/00/0000 00:00:00"));
			return;
		}
		if(!$hh)
		{
			$this->parent->data->set($field, $this->parent->data->reformatToSQLDate("$dd"));
			return;
		}
		
		if ($aa != "AM" && $aa != "PM") throw new FakoliException("Invalid meridiem");
		if ($aa == "PM" && $hh != "12") $hh += 12;
		if ($aa == "AM" && $hh == "12") $hh = "00";
		$this->parent->data->set($field, $this->parent->data->reformatToSQLDate("$dd $hh:$mm:00"));
	}
	
	function format($field)
	{
		return DateTimeTypeRenderer::format($this->parent->data->get($field), $this->template);
	}
	
}

?>