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

class TimestampTypeRenderer extends AbstractTypeRenderer
{
	
	function TimestampTypeRenderer()
	{
	}
	
	/*
	 * If no template, use formatDateShort, otherwise,
	 * format using template provided the date function:
	 * 
	 * http://php.net/manual/en/function.date.php
	 * 
	 * default format: date("Y-m-d H:i:s", strtotime($data));
	 * examples:
	 * 1) {my_timestamp:m/d/Y g:ia} returns 2/12/2011 12:30am
	 * 2) {my_datetime:l F j, Y g:ia} returns long format Monday February 12, 2011 12:30am
	 * 3) {my_datetime:F d, Y g:ia} returns February 12, 2011 12:30am
	 * 
	 * Template shorthand:
	 * {my_timestamp:short} - returns format 1 above
	 * {my_timestamp:daylong} = returns format 2 above
	 * {my_timestamp:long} - returns format 3 above
 	*/
	static function format($timestamp, $template = "")
	{
		trace("TimestampTypeRenderer template is $template and value is $timestamp", 2);
			
		$text = "";
		
		if (!$timestamp) return "N/A";
		
		// This default is needed for XML transation but is not
		// a user-readable format.
		elseif($timestamp && !$template)
			$template = "Y-m-d H:i:s";

		$template = AbstractTypeRenderer::translationDateTemplate($template);
			
		if (preg_match("/^\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}$/", $timestamp))
		{
			$timestamp = preg_replace("/\\D/", "", $timestamp);
		}
		
		if (preg_match("/^\\d{14}$/", $timestamp))
		{
			$yyyy = substr($timestamp,0,4);
			$mm   = substr($timestamp,4,2);
			$dd   = substr($timestamp,6,2);
			$hh   = substr($timestamp,8,2);
			$mi   = substr($timestamp,10,2);
			$ss   = substr($timestamp,12,2);
			
			if ($yyyy == '0000') 
	    		$text = "N/A";
	
	     	if($timestamp)
	  			$text = date($template, mktime($hh,$mi,$ss,$mm,$dd,$yyyy));
		}
		else
		{
			$text = date($template, strftime($timestamp));
		}
							
		return $text; 
	}
}