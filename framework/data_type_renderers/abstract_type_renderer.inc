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

require_once realpath(dirname(__FILE__)."/boolean_type_renderer.inc");
require_once realpath(dirname(__FILE__)."/currency_type_renderer.inc");
require_once realpath(dirname(__FILE__)."/date_type_renderer.inc");
require_once realpath(dirname(__FILE__)."/datetime_type_renderer.inc");
require_once realpath(dirname(__FILE__)."/number_type_renderer.inc");
require_once realpath(dirname(__FILE__)."/phone_number_type_renderer.inc");
require_once realpath(dirname(__FILE__)."/string_type_renderer.inc");
require_once realpath(dirname(__FILE__)."/text_type_renderer.inc");
require_once realpath(dirname(__FILE__)."/time_type_renderer.inc");
require_once realpath(dirname(__FILE__)."/timestamp_type_renderer.inc");
require_once realpath(dirname(__FILE__)."/timezone_type_renderer.inc");

class AbstractTypeRenderer
{		
	function AbstractTypeRenderer()
	{
		
	}
	
	static function format($value, $template = "")
	{

	}
	
	/*
	 * Given a date formatting shorthand, translate into the
	 * format used by the php date function
	 * 
	 * @author Janice Gallant
	 * 
	 * 5/30/2011
	 * 
	 * @param $template - the field format template provided
	 * 
	 * @param $stripTime - whether any time component in the
	 * tempalte should be ignored/removed.
	 */
	static function translationDateTemplate($template, $includeTime = true)
	{
		if($template == "short")
			$template = ($includeTime) ? "m/d/Y g:ia" : "m/d/Y";
		elseif($template == "daylong")
			$template = ($includeTime) ? "l F j, Y g:ia" : "l F j, Y";
		elseif($template == "long")
			$template = ($includeTime) ? "F d, Y g:ia" : "F d, Y";
	
		return $template;	
	}
}