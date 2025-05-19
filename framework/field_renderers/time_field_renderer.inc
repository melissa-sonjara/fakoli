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
 * Field renderer for time data fields. 
 */
class TimeFieldRenderer extends FieldRenderer
{
	function __construct(&$parent)
	{
		parent::__construct($parent);		
	}
	
	function writeScript($field)
	{
		//TODO: Script to mask for correct input
	}
	
	function renderField($field)
	{
		$value = $this->parent->data->get($field);

		// JDG show as empty if value stored is 00
		if ($value AND $value != '00:00:00')
		{
			$text = date("h/i/A", strtotime($value));
			list($hh, $mm, $aa) = explode("/", $text);
		}
	 
		$this->_startField($field);
		
		echo "<input type='text' value='$hh' name='{$field}_hh' size='2' maxlength='2' onkeydown='return maskInput(event);'>&nbsp;:&nbsp;<input type='text' value='$mm' name='{$field}_mm' size='2' maxlength='2' onkeydown='return maskInput(event);'>";
		echo "&nbsp;<select name='{$field}_aa'><option";
		if ($aa == 'AM') echo " selected";
		echo ">AM</option><option";
		if ($aa == 'PM') echo " selected";
		echo ">PM</option></select>";
		
		$this->_endField($field);
	}
	
	function addValidatorsToForm($field, $required = false)
	{
		$prettyName = $this->parent->prettifyFieldName($field);

		$this->parent->validator->add(new TimeValidator($field, $label));
				
		if($required)
		{
			$this->parent->validator->add(new RequiredTimeValidator($field, $label));
		}		
	}
	
	function renderSearchField($field, $mode)
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
		$this->_startField($field);

		//echo date('h:i A', strttotime($this->parent->data->get($field)));

		echo $this->parent->data->format("{{$field}:h:i A}");
		
		$this->_endField($field);
	}
	
	function preProcess($field = "")
	{
		$hh = checkNumeric($_POST["{$field}_hh"]);
		$mm = checkNumeric($_POST["{$field}_mm"]);
		$aa = $_POST["{$field}_aa"];
		if ($aa != "AM" && $aa != "PM") die("Invalid meridiem");

		// JDG - allow time fields to be empty
		if(!$hh)
		{
			$this->parent->data->set($field, $this->parent->data->reformatToSQLDate("00:00"));
			return;
		}
	
		if ($aa == "PM" && $hh != "12") $hh += 12;
		if ($aa == "AM" && $hh == "12") $hh = "00";
		$this->parent->data->set($field, "$hh:$mm");
	}
}

/**@}*/?>