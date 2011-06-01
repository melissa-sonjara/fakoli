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

class BooleanTypeRenderer extends AbstractTypeRenderer
{
	function BooleanTypeRenderer()
	{
	}
	
	/*
	 * If no template, then return True/False. If there is a template
	 * but missing a value for true or false, then return True/False.
	 * 
	 * Template should be provided in the format:
	 * {my_field_name:StringForTrue/StringForFalse}
	 * e.g.,
	 * {active:Yes/No} returns Yes if active and No if not
	 */
	static function format($value, $template = "")
	{
		trace("DateTimeTypeRenderer template is $template and value is $value", 3);

		if($template)
		{
			list($true, $false) = explode("/", $template);
			if($true && $false)		
				$text = ($value) ? $true : $false;
		}
		
		if(!$text)
			$text = ($value) ? "True" : "False";
				
		return $text; 
	}
}