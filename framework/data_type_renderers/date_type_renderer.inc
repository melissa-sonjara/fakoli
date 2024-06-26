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

class DateTypeRenderer extends AbstractTypeRenderer
{
	
	function DateTypeRenderer()
	{
	}
	
	/*
	 * If no template, use formatDateShort, otherwise,
	 * format using template provided the date function:
	 * 
	 * http://php.net/manual/en/function.date.php
	 * 
	 * examples:
	 * {my_date:m/d/Y} returns the default 2/12/11
	 * {my_date:l F j, Y} returns long format Monday February 12, 2011
	 * {my_date:F d, Y} returns February 12, 2011
	 * 
	 * Template shorthand:
	 * {my_date:short} - returns format 1 above
	 * {my_date:daylong} = returns format 2 above
	 * {my_date:long} - returns format 3 above
	 * 
	 */
	static function format($date, $template = "")
	{		
		$text = "";
		
		if($date && !$template)
			$template = "m/d/Y";
		$template = AbstractTypeRenderer::translationDateTemplate($template, false);
 		
		// Blank for zero dates
		if (!strncmp("0000-00-00", $date, 10)) return "";
		
		if($date)
		{
			$dateObj = new DateTime($date);
			$text = $dateObj->format($template);
		}
		
		// return blank if no date
		return $text; 
	}
}

/**@}*/