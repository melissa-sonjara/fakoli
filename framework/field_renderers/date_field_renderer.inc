<?php
/**\addtogroup fieldrenderers
 * @{
 */

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
class DateFieldRenderer extends FieldRenderer
{
	var $validator;
	var $onChange = ""; 	///< optional javascript callback for 'change' event on date text field
	var $onInput = "";		///< optional javascript callback for 'input' event on date text field
	var $template = ""; 	///< template for date display in readonly mode - default reaonly format is short style
	
	function DateFieldRenderer(&$parent)
	{
		$this->FieldRenderer($parent);
	}
	
	function addValidatorsToForm($field, $required = false)
	{
		$label = isset($this->label) ? $this->label : $this->parent->prettifyFieldName($field);
		
		$this->parent->validator->add(new DateValidator($field, $label));
		
		if($required)
		{
			$this->parent->validator->add(new RequiredValidator($field, $label));
		}
		
	}
	
	function renderScript($field)
	{
		$script = "";
		
//		if (!$this->_includedDateScript)
//		{
//			echo "<script src='{$this->parent->componentPath}/calendar/sonjara_calendar.js' type='text/javascript'></script>\n";
//			echo "<link href='{$this->parent->componentPath}/calendar/sonjara_calendar.css' type='text/css' rel='stylesheet'>\n";
//			$this->_includedDateScript = true;
//		}
		
		$calendar = "{$this->parent->id}_{$field}_calendar";
		echo "<script type='text/javascript'>\n";
		echo "var $calendar = new Calendar('$calendar', '', '{$this->parent->id}_{$field}');\n";
		echo "</script>\n";
	}

	function renderField($field)
	{
		$obj = $this->parent->data;
		if ($obj instanceof CompositeDataItem)
		{
			$obj = $obj->findSubObject($field);
		}
		$date = $obj->format("{{$field}:m/d/Y}");
		
		$onchange = ($this->onChange) ? "onchange='$this->onChange(this)'" : "";
		$oninput  = ($this->onInput) ? "oninput='$this->onInput(this)'" : "";
		
		$this->_startField($field);
		echo "<input id='{$this->parent->id}_{$field}' type='text' name='$field' value='$date' $onchange $oninput style='width: auto' class='date' size='12'>&nbsp;<img src='{$this->parent->componentPath}/calendar/calendar.gif' alt='Popup Calendar' align='absmiddle' border='0' onclick='{$this->parent->id}_{$field}_calendar.toggle(this)'>\n";
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
		$this->_startField($field);
		
		if ($mode == "range")
		{
			$label = $this->parent->layout->getLabel($field, $this);
			$toLabel = (preg_match("/Between/i", $label)) ? "and" : "to";
			
			$date_from = $this->parent->params->get($field, "from");
			$date_to = $this->parent->params->get($field, "to");
			echo "<input type='text' name='$field:from' value='$date_from' class='date' size='12'>&nbsp;<img src='{$this->parent->componentPath}/calendar/calendar.gif' alt='Popup Calendar' align='absmiddle' border='0' onclick='{$field}_calendar_from.toggle(this)'>";
			echo "&nbsp;<label for='date_to'>$toLabel</label>&nbsp;";
			echo "<input type='text' name='$field:to' value='$date_to' class='date' size='12'>&nbsp;<img src='{$this->parent->componentPath}/calendar/calendar.gif' alt='Popup Calendar' align='absmiddle' border='0' onclick='{$field}_calendar_to.toggle(this)'>\n";
		}
		else
		{
			$date = $this->parent->params->get($field, $mode);

			echo "<input type='text' name='$field:$mode' value='$date' class='date' size='12'>&nbsp;<img src='{$this->parent->componentPath}/calendar/calendar.gif' alt='Popup Calendar' align='absmiddle' border='0' onclick='{$field}_calendar_{$mode}.toggle(this)'>\n";
		}
		
		$this->_endField($field);
	}
	
	function renderReadOnly($field)
	{
		$this->_startField($field);

		echo $this->format($this->parent->data->get($field));

		$this->_endField($field);
	}
	
	function renderDateRangeReadOnly($field, $date_from, $date_to)
	{
		$label = $this->parent->layout->getLabel($field, $this);
		
		if($date_from && $date_to)
		{
			$toLabel = (preg_match("/Between/i", $label)) ? "and" : "to";
		}
		else if($date_from && !$date_to)
		{
			$this->label = preg_replace("/Between/i", "From", $label);
		}
		else // date to and no date_from
		{
			$this->label = preg_replace("/(Between)|(From)/i", "Up To", $label);		
		}
		echo "<tr>\n";
		$this->_printLabel($field);
		echo "<td";
		if ($this->parent->valueCSS) echo " class='{$this->parent->valueCSS}'";
		echo ">".$this->format($date_from). "&nbsp;<label for='date_to'>$toLabel</label>&nbsp;" . $this->format($date_to) ."</td>\n";
		echo "</tr>\n";		
	}

	function format($value)
	{
		return DateTypeRenderer::format($value, $this->template);
	}
}

/**@}*/?>