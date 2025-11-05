<?php
/**\addtogroup datatyperenderers
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

require_once realpath(dirname(__FILE__)."/abstract_type_renderer.inc");

class DateTimeTypeRenderer extends AbstractTypeRenderer
{
	
	function DateTimeTypeRenderer()
	{
	}
	
	/*
	 * If no template, use formatDateShort, otherwise,
	 * format using template provided the date function:
	 * 
	 * http://php.net/manual/en/function.date.php
	 * 
	 * default: "m/d/Y g:ia";
	 * 
	 * examples:
	 * {my_datetime} returns the default 2/12/2011 12:30am
	 * {my_datetime:l F j, Y g:ia} returns long format Monday February 12, 2011 12:30am
	 * {my_datetime:F d, Y g:ia} returns February 12, 2011 12:30am
	 * 
 	 * Template shorthand:
	 * {my_timestamp:short} - returns format 1 above
	 * {my_timestamp:daylong} = returns format 2 above
	 * {my_timestamp:long} - returns format 3 above
 	 * 
 	*/
	static function format($datetime, $template = "")
	{
		trace("DateTimeTypeRenderer template is $template and value is $datetime", 5);
		
		// handle zero date / datetime
		if ($datetime == "0000-00-00 00:00:00" || $datetime == "0000-00-00")
		{
			$datetime = null;
		}

		$text = "";
			
		if($datetime && !$template)
			$template = "m/d/Y g:ia";

		// Convert shorthand to format needed for date function
		$template = AbstractTypeRenderer::translationDateTemplate($template);
		
		// strip time component if hours is empty
		if(preg_match("/00:00:00/", $datetime))
		{		
			$patterns = "/(g|i|a|:|h|s|b|p|t|z|c|r|u)/i";
    		$template = preg_replace($patterns, "", $template);
			trace("DateTimeTypeRenderer no time component: template is $template and value is $datetime", 5);
		}

		trace("DateTimeTypeRenderer template is $template and value is $datetime", 5);
		
		if($datetime)
		{
			$dateObj = new DateTime($datetime);
			$text = $dateObj->format($template);
		}
		// else if no date return "";
			
		return $text; 
	}
}

/**@}*/
