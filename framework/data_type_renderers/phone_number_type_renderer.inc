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

/*
 * $parts is used by PhoneNumberFieldRenderer
 * 
 * standard rendering is in the format
 * (123) 456-7890 ext 12
 */
class PhoneNumberTypeRenderer extends AbstractTypeRenderer
{	
	static $parts = array(
			"area_code"	=> 3, 
			"exchange"	=> 3,
			""			=> 4, // leave local digits as base field name for RequiredValidator test 
			"extension"	=> 6
			);
	
	
	function PhoneNumberTypeRenderer()
	{
	}
	
	static function format($value, $template = "")
	{			
		trace("PhoneNumberTypeRenderer function format value is $value", 4);
		switch($template)
		{

		case "standard":
			return PhoneNumberTypeRenderer::standard($value);
			
		default:
			
			return PhoneNumberTypeRenderer::standard($value);
		}
		
		return $value; 
	}
	
	static function standard($value)
	{
		trace("PhoneNumberTypeRenderer function standard value is $value", 4);
		$valueParts = PhoneNumberTypeRenderer::getValueParts($value);
		
		$phone = implode($valueParts, "-");
		
		$area_code = $valueParts["area_code"];
		$ext = $valueParts["extension"];
		$phone = preg_replace("/^$area_code-/", "($area_code) ", $phone);
		if($ext)
			$phone = preg_replace("/-$ext$/", " ext $ext", $phone);
		else
			$phone = preg_replace("/-*$/", "", $phone);

		return $phone;
	} 
	
	/*
	 * Given a string of just digits, break into
	 * a value for each phone parts area code,
	 * exchange, local, and extension).
	 */
	static function getValueParts($value)
	{
		$parts = PhoneNumberTypeRenderer::$parts;
		
		$valueParts = array();
		
		$idx = 0;
		foreach($parts as $part => $limit)
		{
			$valueParts[$part] = substr($value, $idx, $limit);
			$idx += $limit;
		}
		
		return $valueParts;
	}
}
