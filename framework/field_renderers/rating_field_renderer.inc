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
 * Field renderer for numeric rating data fields. Renders as a series
 * of radio buttons laid out horizontally, between minimum and maximum labels.
 * Useful for surveys.
 */
class RatingFieldRenderer extends FieldRenderer
{
	var $ranks;
	var $minLabel;
	var $maxLabel;
	
	function __construct(&$form)
	{
		$this->ranks = 5;
		$this->minLabel = "(little)";
		$this->maxLabel = "(very)";
		
		parent::__construct($form);
		$this->colspan = 2;
		
		//$form->override($field, $label, $this);
	}
	
	function renderScript($field)
	{
		// No output
		return "";
	}
	
	function renderField($field)
	{
		$this->_startField($field);
		
		echo "{$this->minLabel}&nbsp;&nbsp;";
		$current = $this->parent->data->get($field);
		
		for($i = 1; $i <= $this->ranks; ++$i)
		{
			$checked = ($current == $i) ? " checked='checked'" : "";
			echo "<input type='radio' name='$field' value='$i'$checked>&nbsp;$i&nbsp;&nbsp;&nbsp;";
		}
		
		echo "{$this->maxLabel}";

		$this->_endField($field);
	}
	
	
	function renderReadOnly($field)
	{
		$this->_startField($field);

		echo $this->parent->data->get($field);
		
		$this->_endField($field);
	}

	function renderSearchField($field, $mode = "equal")
	{
		$this->renderField($field);
	}
}	

/**@}*/?>