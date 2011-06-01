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

require_once realpath(dirname(__FILE__)."/abstract_type_renderer.inc");

class CurrencyTypeRenderer extends AbstractTypeRenderer
{
	
	function CurrencyTypeRenderer()
	{
	}
	
	/*
	 * For currency formatting, the template should be 
	 * a number, reprenting the decimal parameter to the
	 * function number_format.
	 * 
	 * If no template, formats with 2 decimals.
	 * 
	 * A future enhancement would be to allow for
	 * additional decimal format options such as
	 * placement of ",". However, it can wait until needed.
	 * 
	 * http://php.net/manual/en/function.number-format.php
	 * 
	 * For example:
	 * {amount:0} returns 89
	 * {amount:1} returns 89.3
	 * {amount:2} returns 89.32
	 */
	static function format($value, $template = "")
	{
		$template = trim($template);
	
		if ($value == "" || !is_numeric($value))
			$text = "0";				
		elseif($template != "" && is_numeric($template))
			$text = number_format($value, $template);

		if(!$text)
			$text = number_format($value, 2);
		
		return $text; 
	}
}